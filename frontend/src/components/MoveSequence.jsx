/**
 * Scrollable horizontal list of move notation strings.
 */
export default function MoveSequence({ moves }) {
  if (!moves || moves.length === 0) {
    return (
      <div className="flex items-center justify-center py-6 rounded-xl bg-surface-900/30 border border-dashed border-surface-800/50">
        <p className="text-xs text-surface-700 font-medium">
          No moves — cube is already solved or awaiting solve
        </p>
      </div>
    );
  }

  return (
    <div className="space-y-2 animate-fade-in">
      <div className="flex items-center justify-between">
        <h3 className="text-xs font-bold uppercase tracking-widest text-surface-700">
          Solution
        </h3>
        <span className="text-[10px] font-mono text-surface-700">
          {moves.length} move{moves.length !== 1 ? 's' : ''}
        </span>
      </div>

      <div className="flex flex-wrap gap-1.5 max-h-32 overflow-y-auto p-3 rounded-xl bg-surface-900/40 border border-surface-800/40">
        {moves.map((move, i) => (
          <span
            key={i}
            className="inline-flex items-center justify-center min-w-[2.2rem] px-2 py-1 rounded-lg bg-surface-800/60 border border-surface-700/30 text-xs font-mono font-semibold text-surface-100 transition-colors hover:bg-primary/15 hover:text-primary-light hover:border-primary/30"
            style={{ animationDelay: `${i * 30}ms` }}
          >
            {move}
          </span>
        ))}
      </div>
    </div>
  );
}
