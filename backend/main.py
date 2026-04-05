"""
Rubik's Cube Solver — FastAPI Backend

Endpoints
---------
POST /solve      — Solve a single cube state with a specified algorithm.
POST /benchmark  — Run all 3 algorithms across scramble depths [3..7].
GET  /scan       — Placeholder for Phase 2 computer-vision scan.
GET  /health     — Liveness probe.
"""

from __future__ import annotations

import json
import subprocess
from pathlib import Path
from typing import Any

from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel, Field, field_validator

from solver_bridge import call_solver

# ─── App ────────────────────────────────────────────────────────
app = FastAPI(
    title="Rubik's Cube Solver API",
    version="1.0.0",
    description="Solve and benchmark Rubik's Cube states using BFS, A*, and IDA*.",
)

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# ─── Request / Response Models ──────────────────────────────────

class SolveRequest(BaseModel):
    """Payload for the ``POST /solve`` endpoint."""

    state: str = Field(
        ...,
        min_length=54,
        max_length=54,
        description="54-character cube state (URFDLB encoding).",
    )
    algo: str = Field(
        ...,
        description="Algorithm: bfs, astar, or idastar.",
    )
    heuristic: str = Field(
        default="misplaced",
        description="Heuristic function: misplaced or manhattan.",
    )

    @field_validator("state")
    @classmethod
    def validate_state_chars(cls, v: str) -> str:
        valid_chars = set("URFDLB")
        invalid = set(v) - valid_chars
        if invalid:
            raise ValueError(f"Invalid characters in state: {invalid}")
        return v

    @field_validator("algo")
    @classmethod
    def validate_algo(cls, v: str) -> str:
        allowed = {"bfs", "astar", "idastar"}
        if v not in allowed:
            raise ValueError(f"algo must be one of {allowed}")
        return v

    @field_validator("heuristic")
    @classmethod
    def validate_heuristic(cls, v: str) -> str:
        allowed = {"misplaced", "manhattan"}
        if v not in allowed:
            raise ValueError(f"heuristic must be one of {allowed}")
        return v


class SolveResponse(BaseModel):
    """Response from ``POST /solve``."""

    moves: list[str]
    nodes: int
    time_ms: float
    mem_kb: int


class BenchmarkEntry(BaseModel):
    """Single row in the benchmark results array."""

    depth: int
    algo: str
    moves: list[str]
    move_count: int
    nodes: int
    time_ms: float
    mem_kb: int


class ScanResponse(BaseModel):
    """Placeholder response for the Phase 2 scan endpoint."""

    state: str


class HealthResponse(BaseModel):
    """Liveness response."""

    status: str


# ─── Helpers ────────────────────────────────────────────────────

_SOLVER_BINARY = Path(__file__).resolve().parent.parent / "build" / "rubiks_solver"
_ALGOS = ["bfs", "astar", "idastar"]
_BENCHMARK_DEPTHS = [3, 4, 5, 6, 7]



def _run_benchmark_entry(
    depth: int,
    algo: str,
    seed: int = 42,
) -> dict[str, Any]:
    """Run a single benchmark: scramble at `depth`, solve with `algo`."""
    solver_path = str(_SOLVER_BINARY)
    if not _SOLVER_BINARY.is_file():
        raise HTTPException(
            status_code=500,
            detail=f"Solver binary not found at {solver_path}.",
        )

    cmd = [
        solver_path,
        "--scramble", str(depth),
        "--seed", str(seed),
        "--algo", algo,
        "--heuristic", "misplaced",
    ]

    try:
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            timeout=65,
        )
    except subprocess.TimeoutExpired:
        return {
            "depth": depth,
            "algo": algo,
            "moves": [],
            "move_count": 0,
            "nodes": 0,
            "time_ms": -1.0,
            "mem_kb": 0,
            "error": f"Timed out after 65s",
        }
    except OSError as exc:
        raise HTTPException(status_code=500, detail=f"Benchmark failed: {exc}")

    if result.returncode != 0:
        return {
            "depth": depth,
            "algo": algo,
            "moves": [],
            "move_count": 0,
            "nodes": 0,
            "time_ms": -1.0,
            "mem_kb": 0,
            "error": (result.stderr or "").strip()[:200],
        }


    try:
        data = json.loads(result.stdout.strip())
    except Exception:
        return {
            "depth": depth,
            "algo": algo,
            "moves": [],
            "move_count": 0,
            "nodes": 0,
            "time_ms": -1.0,
            "mem_kb": 0,
            "error": "Invalid JSON from solver",
        }

    return {
        "depth": depth,
        "algo": algo,
        "moves": data.get("moves", []),
        "move_count": len(data.get("moves", [])),
        "nodes": data.get("nodes", 0),
        "time_ms": data.get("time_ms", 0.0),
        "mem_kb": data.get("mem_kb", 0),
    }


# ─── Endpoints ──────────────────────────────────────────────────

@app.post("/solve", response_model=SolveResponse)
async def solve(req: SolveRequest) -> SolveResponse:
    """Solve a single cube state.

    Calls the C++ solver binary via subprocess and returns the result.
    """
    data = call_solver(
        state=req.state,
        algo=req.algo,
        heuristic=req.heuristic,
    )
    return SolveResponse(
        moves=data["moves"],
        nodes=data["nodes"],
        time_ms=data["time_ms"],
        mem_kb=data["mem_kb"],
    )


@app.post("/benchmark")
async def benchmark() -> list[dict[str, Any]]:
    """Run all 3 algorithms across scramble depths [3, 4, 5, 6, 7].

    Uses a fixed seed (42) for reproducibility so the same scramble is used
    for every algorithm at a given depth, enabling fair comparison.

    Returns a flat JSON array of benchmark entries.
    """
    results: list[dict[str, Any]] = []

    for depth in _BENCHMARK_DEPTHS:
        for algo in _ALGOS:
            entry = _run_benchmark_entry(depth=depth, algo=algo, seed=42)
            results.append(entry)

    return results


@app.get("/scan", response_model=ScanResponse)
async def scan() -> ScanResponse:
    """Phase 2 placeholder — computer-vision cube scanning."""
    return ScanResponse(state="not_implemented")


@app.get("/health", response_model=HealthResponse)
async def health() -> HealthResponse:
    """Liveness probe."""
    return HealthResponse(status="ok")
