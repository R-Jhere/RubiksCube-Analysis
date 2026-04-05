const API_BASE = 'http://localhost:8000';

/**
 * Solve a single cube state.
 *
 * @param {{ state: string, algo: string, heuristic: string }} params
 * @returns {Promise<{ moves: string[], nodes: number, time_ms: number, mem_kb: number }>}
 */
export async function solveCube({ state, algo, heuristic = 'misplaced' }) {
  const res = await fetch(`${API_BASE}/solve`, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ state, algo, heuristic }),
  });

  if (!res.ok) {
    const err = await res.json().catch(() => ({}));
    throw new Error(err.detail || `Solver error (HTTP ${res.status})`);
  }

  return res.json();
}

/**
 * Run benchmark across all algorithms and depths.
 *
 * @returns {Promise<Array<{ depth: number, algo: string, moves: string[], move_count: number, nodes: number, time_ms: number, mem_kb: number }>>}
 */
export async function runBenchmark() {
  const res = await fetch(`${API_BASE}/benchmark`, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
  });

  if (!res.ok) {
    const err = await res.json().catch(() => ({}));
    throw new Error(err.detail || `Benchmark error (HTTP ${res.status})`);
  }

  return res.json();
}
