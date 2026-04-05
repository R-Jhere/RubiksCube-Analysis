const METRICS = [
  {
    key: 'time_ms',
    label: 'Time',
    format: (v) => (v < 1 ? `${(v * 1000).toFixed(0)}µs` : `${v.toFixed(1)}ms`),
    icon: (
      <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
        <circle cx="12" cy="12" r="10" />
        <polyline points="12 6 12 12 16 14" />
      </svg>
    ),
    color: 'text-primary-light',
    bg: 'bg-primary/10',
  },
  {
    key: 'nodes',
    label: 'Nodes',
    format: (v) => (v >= 1_000_000 ? `${(v / 1_000_000).toFixed(1)}M` : v >= 1_000 ? `${(v / 1_000).toFixed(1)}K` : String(v)),
    icon: (
      <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
        <circle cx="12" cy="12" r="3" />
        <path d="M12 2v4m0 12v4M2 12h4m12 0h4M4.93 4.93l2.83 2.83m8.48 8.48l2.83 2.83M4.93 19.07l2.83-2.83m8.48-8.48l2.83-2.83" />
      </svg>
    ),
    color: 'text-accent-light',
    bg: 'bg-accent/10',
  },
  {
    key: 'mem_kb',
    label: 'Memory',
    format: (v) => (v >= 1024 ? `${(v / 1024).toFixed(1)}MB` : `${v}KB`),
    icon: (
      <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
        <rect x="4" y="4" width="16" height="16" rx="2" />
        <rect x="9" y="9" width="6" height="6" />
        <path d="M9 2v2m6-2v2M9 20v2m6-2v2M2 9h2m-2 6h2M20 9h2m-2 6h2" />
      </svg>
    ),
    color: 'text-warning',
    bg: 'bg-warning/10',
  },
  {
    key: 'move_count',
    label: 'Moves',
    format: (v) => String(v),
    icon: (
      <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
        <path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z" />
        <polyline points="14 2 14 8 20 8" />
        <line x1="16" y1="13" x2="8" y2="13" />
        <line x1="16" y1="17" x2="8" y2="17" />
      </svg>
    ),
    color: 'text-surface-200',
    bg: 'bg-surface-800',
  },
];

/**
 * 4-tile metrics display showing solve results.
 */
export default function MetricsCard({ result }) {
  if (!result) {
    return (
      <div className="grid grid-cols-2 gap-2">
        {METRICS.map((m) => (
          <div
            key={m.key}
            className="flex flex-col items-center justify-center gap-1 py-4 rounded-xl bg-surface-900/40 border border-surface-800/40"
          >
            <span className="text-surface-700">{m.icon}</span>
            <span className="text-[10px] font-semibold text-surface-700 uppercase tracking-wider">
              {m.label}
            </span>
            <span className="text-sm font-mono text-surface-800">—</span>
          </div>
        ))}
      </div>
    );
  }

  // Derive move_count from moves array
  const data = { ...result, move_count: result.moves?.length ?? 0 };

  return (
    <div className="grid grid-cols-2 gap-2 animate-fade-in">
      {METRICS.map((m) => (
        <div
          key={m.key}
          className={`flex flex-col items-center justify-center gap-1 py-4 rounded-xl ${m.bg} border border-surface-800/30 glass-card-hover`}
        >
          <span className={m.color}>{m.icon}</span>
          <span className="text-[10px] font-semibold text-surface-700 uppercase tracking-wider">
            {m.label}
          </span>
          <span className={`text-lg font-bold font-mono ${m.color}`}>
            {m.format(data[m.key])}
          </span>
        </div>
      ))}
    </div>
  );
}
