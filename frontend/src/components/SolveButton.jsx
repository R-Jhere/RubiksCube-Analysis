/**
 * Spinner icon for loading states.
 */
function Spinner() {
  return (
    <svg
      className="animate-spin h-4 w-4"
      viewBox="0 0 24 24"
      fill="none"
    >
      <circle
        cx="12" cy="12" r="10"
        stroke="currentColor"
        strokeWidth="3"
        strokeDasharray="32"
        strokeLinecap="round"
        className="opacity-30"
      />
      <path
        d="M12 2a10 10 0 0 1 10 10"
        stroke="currentColor"
        strokeWidth="3"
        strokeLinecap="round"
      />
    </svg>
  );
}

/**
 * Primary solve button with loading spinner.
 */
export default function SolveButton({ onClick, loading, disabled }) {
  return (
    <button
      id="solve-button"
      type="button"
      onClick={onClick}
      disabled={disabled || loading}
      className={`
        w-full flex items-center justify-center gap-2.5
        px-6 py-3.5 rounded-xl
        text-sm font-bold tracking-wide
        transition-all duration-200 ease-out
        cursor-pointer
        ${
          disabled || loading
            ? 'bg-surface-800 text-surface-700 cursor-not-allowed'
            : 'bg-gradient-to-r from-primary to-primary-dark text-white shadow-lg shadow-primary/25 hover:shadow-primary/40 hover:brightness-110 active:scale-[0.98]'
        }
      `}
    >
      {loading ? (
        <>
          <Spinner />
          Solving…
        </>
      ) : (
        <>
          <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2.5" strokeLinecap="round" strokeLinejoin="round">
            <polygon points="5 3 19 12 5 21 5 3" />
          </svg>
          Solve
        </>
      )}
    </button>
  );
}
