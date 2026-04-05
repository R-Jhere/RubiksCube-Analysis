/**
 * Stub panel for Phase 3 benchmarking.
 * Shows a disabled button and placeholder message.
 */
export default function BenchmarkPanel() {
  return (
    <div className="glass-card p-5 space-y-3">
      <div className="flex items-center justify-between">
        <h2 className="text-xs font-bold uppercase tracking-widest text-surface-700">
          Benchmark
        </h2>
        <span className="text-[10px] font-medium text-surface-700 bg-surface-800 px-2 py-0.5 rounded-full">
          Phase 3
        </span>
      </div>

      <p className="text-xs text-surface-700 leading-relaxed">
        Run all algorithms across scramble depths 3–7 and compare performance
        with interactive charts.
      </p>

      <button
        type="button"
        disabled
        className="w-full flex items-center justify-center gap-2 px-4 py-3 rounded-xl bg-surface-800/50 border border-surface-800 text-surface-700 text-sm font-semibold cursor-not-allowed"
      >
        <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
          <line x1="18" y1="20" x2="18" y2="10" />
          <line x1="12" y1="20" x2="12" y2="4" />
          <line x1="6" y1="20" x2="6" y2="14" />
        </svg>
        Run Benchmark
      </button>
    </div>
  );
}
