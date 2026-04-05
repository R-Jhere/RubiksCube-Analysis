const ALGORITHMS = [
  { id: 'bfs',    label: 'BFS',   desc: 'Breadth-First Search' },
  { id: 'astar',  label: 'A*',    desc: 'A-Star Search' },
  { id: 'idastar', label: 'IDA*', desc: 'Iterative Deepening A*' },
];

/**
 * Radio group for selecting the solving algorithm.
 */
export default function AlgorithmSelector({ value, onChange }) {
  return (
    <div className="space-y-2">
      <label className="text-sm font-semibold text-surface-200">
        Algorithm
      </label>

      <div className="grid grid-cols-3 gap-2">
        {ALGORITHMS.map((algo) => {
          const isSelected = value === algo.id;
          return (
            <button
              key={algo.id}
              type="button"
              onClick={() => onChange(algo.id)}
              className={`
                relative flex flex-col items-center gap-0.5 px-3 py-3 rounded-xl
                border text-center cursor-pointer
                transition-all duration-200 ease-out
                ${
                  isSelected
                    ? 'bg-primary/12 border-primary/40 shadow-[0_0_12px_oklch(0.65_0.18_250/0.15)]'
                    : 'bg-surface-900/50 border-surface-800 hover:border-surface-700 hover:bg-surface-800/50'
                }
              `}
            >
              <span
                className={`text-sm font-bold tracking-tight ${
                  isSelected ? 'text-primary-light' : 'text-surface-200'
                }`}
              >
                {algo.label}
              </span>
              <span className="text-[10px] text-surface-700 font-medium">
                {algo.desc}
              </span>

              {/* Selection indicator dot */}
              {isSelected && (
                <span className="absolute top-2 right-2 w-2 h-2 rounded-full bg-primary shadow-[0_0_6px_oklch(0.65_0.18_250/0.5)]" />
              )}
            </button>
          );
        })}
      </div>
    </div>
  );
}
