export default function Header() {
  return (
    <header className="flex items-center justify-between px-6 py-4 border-b border-surface-800/60">
      <div className="flex items-center gap-3">
        {/* Cube Icon */}
        <div className="w-9 h-9 rounded-lg bg-gradient-to-br from-primary to-primary-dark flex items-center justify-center shadow-lg shadow-primary/20">
          <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="white" strokeWidth="2.5" strokeLinecap="round" strokeLinejoin="round">
            <path d="M21 16V8a2 2 0 0 0-1-1.73l-7-4a2 2 0 0 0-2 0l-7 4A2 2 0 0 0 3 8v8a2 2 0 0 0 1 1.73l7 4a2 2 0 0 0 2 0l7-4A2 2 0 0 0 21 16z" />
            <polyline points="3.27 6.96 12 12.01 20.73 6.96" />
            <line x1="12" y1="22.08" x2="12" y2="12" />
          </svg>
        </div>

        <div>
          <h1 className="text-lg font-bold tracking-tight text-surface-50">
            Rubik's Cube Solver
          </h1>
          <p className="text-xs text-surface-700 font-medium -mt-0.5">
            DAA Algorithm Analysis
          </p>
        </div>
      </div>

      <span className="inline-flex items-center gap-1.5 px-3 py-1 rounded-full text-xs font-semibold bg-accent/15 text-accent-light border border-accent/20">
        <span className="w-1.5 h-1.5 rounded-full bg-accent animate-pulse" />
        Phase 2
      </span>
    </header>
  );
}
