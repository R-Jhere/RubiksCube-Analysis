"""
Solver Bridge — subprocess wrapper for the C++ Rubik's Cube solver.

Translates FastAPI requests into CLI calls to build/rubiks_solver and parses
the JSON response. This is the ONLY module that touches the filesystem; the
rest of the backend works purely with dicts.
"""

from __future__ import annotations

import json
import subprocess
from pathlib import Path
from typing import Any

from fastapi import HTTPException

# ─── Configuration ──────────────────────────────────────────────
_SOLVER_BINARY = Path(__file__).resolve().parent.parent / "build" / "rubiks_solver"
_TIMEOUT_SECONDS = 65  # generous cap — IDA* can take a while


def call_solver(
    state: str,
    algo: str,
    heuristic: str = "misplaced",
) -> dict[str, Any]:
    """Run the C++ solver as a subprocess and return the parsed JSON result.

    Parameters
    ----------
    state : str
        54-character cube state string (URFDLB encoding).
    algo : str
        Algorithm to use: ``"bfs"``, ``"astar"``, or ``"idastar"``.
    heuristic : str, optional
        Heuristic function name, defaults to ``"misplaced"``.

    Returns
    -------
    dict
        Keys: ``moves`` (list[str]), ``nodes`` (int), ``time_ms`` (float),
        ``mem_kb`` (int).

    Raises
    ------
    HTTPException 500
        If the solver binary is missing, times out, returns a non-zero exit
        code, or produces unparseable output.
    """
    solver_path = str(_SOLVER_BINARY)

    # ── Guard: binary must exist ──
    if not _SOLVER_BINARY.is_file():
        raise HTTPException(
            status_code=500,
            detail=f"Solver binary not found at {solver_path}. Run `make solver` first.",
        )

    cmd = [
        solver_path,
        "--state", state,
        "--algo", algo,
        "--heuristic", heuristic,
    ]

    try:
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            timeout=_TIMEOUT_SECONDS,
        )
    except subprocess.TimeoutExpired:
        raise HTTPException(
            status_code=500,
            detail=f"Solver timed out after {_TIMEOUT_SECONDS}s (algo={algo}).",
        )
    except FileNotFoundError:
        raise HTTPException(
            status_code=500,
            detail=f"Solver binary not found at {solver_path}.",
        )
    except OSError as exc:
        raise HTTPException(
            status_code=500,
            detail=f"Failed to execute solver: {exc}",
        )

    # ── Non-zero exit → surface stderr ──
    if result.returncode != 0:
        stderr_snippet = (result.stderr or "").strip()[:300]
        raise HTTPException(
            status_code=500,
            detail=f"Solver exited with code {result.returncode}: {stderr_snippet}",
        )

    # ── Empty stdout → malformed binary output ──
    stdout = result.stdout.strip()
    if not stdout:
        raise HTTPException(
            status_code=500,
            detail="Solver returned empty stdout.",
        )

    # ── Parse JSON ──
    try:
        data: dict[str, Any] = json.loads(stdout)
    except json.JSONDecodeError as exc:
        raise HTTPException(
            status_code=500,
            detail=f"Solver returned invalid JSON: {exc}",
        )

    # ── Sanity-check expected keys ──
    required_keys = {"moves", "nodes", "time_ms", "mem_kb"}
    missing = required_keys - data.keys()
    if missing:
        raise HTTPException(
            status_code=500,
            detail=f"Solver JSON missing keys: {missing}",
        )

    return data
