import sys
import subprocess
import time
import chess

# --- CONFIGURATION ---
# Path to your compiled executable.
# Windows users: Change to "./engine.exe"
ENGINE_PATH = "./nice.exe" 

# --- HELPER FUNCTIONS ---

def python_perft(board, depth):
    """
    Recursive Perft function using python-chess.
    This provides the 'Ground Truth'.
    """
    if depth == 0:
        return 1
    
    nodes = 0
    # python-chess automatically handles legality checking
    for move in board.legal_moves:
        board.push(move)
        nodes += python_perft(board, depth - 1)
        board.pop()
        
    return nodes

def run_cpp_engine(fen, depth):
    """
    Runs the C++ engine as a subprocess and captures the output.
    """
    try:
        # Calls: ./engine "FEN" DEPTH
        result = subprocess.run(
            [ENGINE_PATH, fen, str(depth)],
            capture_output=True,
            text=True,
            check=True
        )
        # Parse the integer output
        return int(result.stdout.strip())
    except subprocess.CalledProcessError as e:
        print(f"CRITICAL: Engine crashed or returned error code!")
        print(f"Error Output: {e.stderr}")
        return -1
    except ValueError:
        print(f"CRITICAL: Engine returned non-number output: '{result.stdout.strip()}'")
        return -1

def run_test_case(name, fen, depth):
    print(f"==================================================")
    print(f"TEST: {name} (Depth {depth})")
    print(f"FEN:  {fen}")
    
    # 1. Calculate Expected Result (Python)
    print(f"Calculating Ground Truth (Python)... ", end="", flush=True)
    start = time.time()
    board = chess.Board(fen)
    expected = python_perft(board, depth)
    py_time = time.time() - start
    print(f"Done. ({expected} nodes, {py_time:.2f}s)")

    # 2. Run C++ Engine
    print(f"Running C++ Engine...                ", end="", flush=True)
    start = time.time()
    actual = run_cpp_engine(fen, depth)
    cpp_time = time.time() - start
    print(f"Done. ({actual} nodes, {cpp_time:.2f}s)")

    # 3. Compare
    if actual == expected:
        print("RESULT: ✅ PASS")
        # Speed comparison (just for fun)
        speedup = py_time / cpp_time if cpp_time > 0 else 0
        print(f"Speedup: Your engine was {speedup:.1f}x faster than Python.")
    else:
        print("RESULT: ❌ FAIL")
        diff = actual - expected
        print(f"Difference: {diff} nodes")
        if diff > 0:
            print("Hint: You are generating EXTRA moves (likely pseudo-legal moves that are actually illegal).")
        else:
            print("Hint: You are MISSING moves (likely valid moves being flagged as illegal).")

# --- TEST SUITE ---

if __name__ == "__main__":
    # 1. Start Position
    run_test_case("Start Pos", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 3)

    # 2. Position 2 (Strategy)
    run_test_case("Pos 2", "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 3)

    # 3. Position 3 (Endgame / En Passant)
    run_test_case("Pos 3", "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 3)

    # 4. Position 4 (Global Test)
    run_test_case("Pos 4", "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 3)

    # 5. Position 5 (Opposite Side Checks)
    run_test_case("Pos 5", "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 4)