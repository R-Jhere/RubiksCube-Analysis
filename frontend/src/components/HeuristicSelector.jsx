const HEURISTICS = [
  { id: 'misplaced',  label: 'Misplaced Cubies' },
  { id: 'manhattan',  label: 'Manhattan Distance' },
];

/**
 * Radio group for heuristic selection.
 * Disabled when algo is BFS (BFS doesn't use heuristics).
 */
export default function HeuristicSelector({ value, onChange, disabled }) {
  return (
    <div className="space-y-2">
      <div className="flex items-center gap-2">
        <label className="text-sm font-semibold text-surface-200">
          Heuristic
        </label>
        {disabled && (
          <span className="text-[10px] font-medium text-surface-700 bg-surface-800 px-2 py-0.5 rounded-full">
            N/A for BFS
          </span>
        )}
      </div>

      <div className="flex gap-2">
        {HEURISTICS.map((h) => {
          const isSelected = value === h.id;
          return (
            <button
              key={h.id}
              type="button"
              disabled={disabled}
              onClick={() => onChange(h.id)}
              className={`
                flex-1 px-3 py-2.5 rounded-xl border text-sm font-medium
                transition-all duration-200 ease-out
                ${disabled
                  ? 'opacity-40 cursor-not-allowed bg-surface-900/30 border-surface-800 text-surface-700'
                  : isSelected
                    ? 'bg-accent/10 border-accent/30 text-accent-light'
                    : 'bg-surface-900/50 border-surface-800 text-surface-200 hover:border-surface-700 hover:bg-surface-800/50 cursor-pointer'
                }
              `}
            >
              {h.label}
            </button>
          );
        })}
      </div>
    </div>
  );
}
