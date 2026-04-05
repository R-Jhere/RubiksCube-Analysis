import CubeStateInput from './CubeStateInput';
import AlgorithmSelector from './AlgorithmSelector';
import HeuristicSelector from './HeuristicSelector';

/**
 * Left-side configuration panel containing all solver inputs.
 */
export default function InputPanel({
  state,
  onStateChange,
  algo,
  onAlgoChange,
  heuristic,
  onHeuristicChange,
}) {
  const isBfs = algo === 'bfs';

  return (
    <div className="glass-card p-5 space-y-5">
      <h2 className="text-xs font-bold uppercase tracking-widest text-surface-700">
        Configuration
      </h2>

      <CubeStateInput value={state} onChange={onStateChange} />

      <div className="h-px bg-surface-800/60" />

      <AlgorithmSelector value={algo} onChange={onAlgoChange} />

      <HeuristicSelector
        value={heuristic}
        onChange={onHeuristicChange}
        disabled={isBfs}
      />
    </div>
  );
}
