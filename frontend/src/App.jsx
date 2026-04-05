import { useState, useCallback } from 'react';
import Header from './components/Header';
import InputPanel from './components/InputPanel';
import SolvePanel from './components/SolvePanel';
import BenchmarkPanel from './components/BenchmarkPanel';
import { solveCube } from './api/solverApi';

export default function App() {
  // ─── Input State ──────────────────────────────────────────────
  const [cubeState, setCubeState] = useState('');
  const [algo, setAlgo] = useState('bfs');
  const [heuristic, setHeuristic] = useState('misplaced');

  // ─── Solve State ──────────────────────────────────────────────
  const [loading, setLoading] = useState(false);
  const [result, setResult] = useState(null);
  const [error, setError] = useState(null);

  // ─── Validation ───────────────────────────────────────────────
  const isValidState =
    cubeState.length === 54 && /^[URFDLB]+$/.test(cubeState);

  // ─── Solve Handler ────────────────────────────────────────────
  const handleSolve = useCallback(async () => {
    if (!isValidState) return;

    setLoading(true);
    setError(null);
    setResult(null);

    try {
      const data = await solveCube({
        state: cubeState,
        algo,
        heuristic: algo === 'bfs' ? 'misplaced' : heuristic,
      });
      setResult(data);
    } catch (err) {
      setError(err.message || 'An unexpected error occurred.');
    } finally {
      setLoading(false);
    }
  }, [cubeState, algo, heuristic, isValidState]);

  return (
    <div className="min-h-screen flex flex-col">
      <Header />

      <main className="flex-1 p-4 md:p-6 lg:p-8">
        <div className="max-w-5xl mx-auto grid grid-cols-1 md:grid-cols-[1fr_1fr] gap-4 lg:gap-6">
          {/* Left Column — Inputs */}
          <div className="space-y-4">
            <InputPanel
              state={cubeState}
              onStateChange={setCubeState}
              algo={algo}
              onAlgoChange={setAlgo}
              heuristic={heuristic}
              onHeuristicChange={setHeuristic}
            />
            <BenchmarkPanel />
          </div>

          {/* Right Column — Results */}
          <div>
            <SolvePanel
              onSolve={handleSolve}
              loading={loading}
              disabled={!isValidState}
              result={result}
              error={error}
            />
          </div>
        </div>
      </main>

      {/* Footer */}
      <footer className="px-6 py-3 text-center text-[11px] text-surface-700 border-t border-surface-800/40">
        Built with C++ · FastAPI · React — DAA Project 2026
      </footer>
    </div>
  );
}
