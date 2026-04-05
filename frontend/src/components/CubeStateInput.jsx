import { useRef } from 'react';

const GOAL_STATE = 'UUUUUUUUURRRRRRRRR' + 'FFFFFFFFFDDDDDDDDD' + 'LLLLLLLLLBBBBBBBBB';

const FACES = [
  { id: 'U', name: 'Top',    color: 'bg-white',     bgHighlight: 'bg-white/20',     range: '0-8' },
  { id: 'R', name: 'Right',  color: 'bg-red-500',   bgHighlight: 'bg-red-500/20',   range: '9-17' },
  { id: 'F', name: 'Front',  color: 'bg-green-500', bgHighlight: 'bg-green-500/20', range: '18-26' },
  { id: 'D', name: 'Bottom', color: 'bg-yellow-400',bgHighlight: 'bg-yellow-400/20',range: '27-35' },
  { id: 'L', name: 'Left',   color: 'bg-orange-500',bgHighlight: 'bg-orange-500/20',range: '36-44' },
  { id: 'B', name: 'Back',   color: 'bg-blue-500',  bgHighlight: 'bg-blue-500/20',  range: '45-53' },
];

/**
 * 54-char cube state text input with live character counter.
 */
export default function CubeStateInput({ value, onChange }) {
  const charCount = value.length;
  const isValid = charCount === 54 && /^[URFDLB]+$/.test(value);
  const hasInvalidChars = value.length > 0 && !/^[URFDLB]*$/.test(value);

  const inputRef = useRef(null);
  const highlightRef = useRef(null);

  const handleScroll = (e) => {
    if (highlightRef.current) {
      highlightRef.current.scrollLeft = e.target.scrollLeft;
    }
  };

  return (
    <div className="space-y-4">
      <div className="space-y-2">
        <div className="flex items-center justify-between">
          <label
            htmlFor="cube-state-input"
            className="text-sm font-semibold text-surface-200"
          >
            Cube State
          </label>
          <span
            className={`text-xs font-mono font-medium tabular-nums ${
              charCount === 54
                ? 'text-success'
                : charCount > 54
                  ? 'text-error'
                  : 'text-surface-700'
            }`}
          >
            {charCount}/54
          </span>
        </div>

        <div
          className={`
            relative w-full rounded-xl border
            transition-all duration-150
            bg-surface-900/80
            ${
              hasInvalidChars
                ? 'border-error/50 focus-within:ring-2 focus-within:ring-error/30 text-error'
                : isValid
                  ? 'border-success/30 focus-within:ring-2 focus-within:ring-success/30 text-surface-100'
                  : 'border-surface-800 focus-within:ring-2 focus-within:ring-primary/30 text-surface-100'
            }
          `}
        >
          {/* Background Highlight Layer */}
          <div
            ref={highlightRef}
            className="absolute inset-0 px-4 py-3 font-mono text-sm tracking-wider pointer-events-none whitespace-nowrap overflow-hidden text-transparent"
            aria-hidden="true"
          >
            {FACES.map((face, i) => {
              const chunk = value.slice(i * 9, i * 9 + 9);
              if (!chunk) return null;
              return (
                <span key={face.id} className={`${face.bgHighlight} rounded-sm`}>
                  {chunk}
                </span>
              );
            })}
          </div>

          <input
            ref={inputRef}
            id="cube-state-input"
            type="text"
            value={value}
            onChange={(e) => onChange(e.target.value.toUpperCase())}
            onScroll={handleScroll}
            placeholder={GOAL_STATE}
            maxLength={60}
            spellCheck={false}
            autoComplete="off"
            className="relative w-full px-4 py-3 bg-transparent border-none outline-none focus:ring-0 font-mono text-sm tracking-wider placeholder:text-surface-700/50"
          />

          {/* Quick-fill goal state button */}
          {value.length === 0 && (
            <button
              type="button"
              onClick={() => onChange(GOAL_STATE)}
              className="absolute right-3 top-1/2 -translate-y-1/2 text-[10px] font-semibold text-primary-light hover:text-primary transition-colors px-2 py-1 rounded-md hover:bg-primary/10 z-10"
            >
              GOAL
            </button>
          )}
        </div>

        {hasInvalidChars && (
          <p className="text-xs text-error flex items-center gap-1">
            <svg width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2">
              <circle cx="12" cy="12" r="10" />
              <line x1="15" y1="9" x2="9" y2="15" />
              <line x1="9" y1="9" x2="15" y2="15" />
            </svg>
            Only U, R, F, D, L, B characters allowed
          </p>
        )}
      </div>

      {/* Face Reference Grid */}
      <div className="grid grid-cols-2 lg:grid-cols-3 gap-2">
        {FACES.map(face => (
          <div key={face.id} className="flex items-center gap-2.5 p-2.5 rounded-lg bg-surface-900/40 border border-surface-800/60">
            <div className={`w-3.5 h-3.5 rounded-[3px] shrink-0 ${face.color} shadow-sm`} />
            <div className="flex flex-col">
              <div className="text-[11px] font-bold text-surface-200 leading-tight">
                {face.id} <span className="text-surface-700 font-medium">({face.name})</span>
              </div>
              <div className="text-[9px] font-mono text-surface-500 leading-tight mt-0.5">
                {face.range}
              </div>
            </div>
          </div>
        ))}
      </div>
    </div>
  );
}
