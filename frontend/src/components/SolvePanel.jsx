import SolveButton from './SolveButton';
import MetricsCard from './MetricsCard';
import MoveSequence from './MoveSequence';

/**
 * Right-side panel showing the solve button, result metrics, and move sequence.
 */
export default function SolvePanel({
  onSolve,
  loading,
  disabled,
  result,
  error,
}) {
  return (
    <div className="glass-card p-5 space-y-4">
      <h2 className="text-xs font-bold uppercase tracking-widest text-surface-700">
        Results
      </h2>

      <SolveButton onClick={onSolve} loading={loading} disabled={disabled} />

      {/* Error display */}
      {error && (
        <div className="flex items-start gap-2 px-4 py-3 rounded-xl bg-error/10 border border-error/20 animate-fade-in">
          <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" className="text-error mt-0.5 shrink-0">
            <circle cx="12" cy="12" r="10" />
            <line x1="12" y1="8" x2="12" y2="12" />
            <line x1="12" y1="16" x2="12.01" y2="16" />
          </svg>
          <p className="text-xs text-error leading-relaxed">{error}</p>
        </div>
      )}

      <MetricsCard result={result} />

      <MoveSequence moves={result?.moves} />
    </div>
  );
}
