# NICE Chess Engine

A UCI-compliant chess engine written in C++ with a focus on performance and clean architecture. NICE (Not Intelligent Chess Engine) implements modern chess programming techniques including bitboard representation, alpha-beta pruning, and sophisticated move ordering.

## Author

**Tarique Mackay**

## Features

### Core Functionality
- **UCI Protocol Support**: Full compatibility with UCI-compliant chess GUIs (Arena, Cute Chess, etc.)
- **Bitboard Representation**: Efficient 64-bit board representation for fast move generation
- **Legal Move Generation**: Complete generation of all legal moves including special moves (castling, en passant, promotions)
- **FEN Parsing**: Support for loading positions from FEN (Forsyth-Edwards Notation) strings

### Search & Evaluation
- **Negamax Algorithm**: Principal variation search with alpha-beta pruning
- **Quiescence Search**: Extended search for capture sequences to avoid horizon effect
- **Iterative Deepening**: Progressive depth search with time management
- **Move Ordering**: MVV-LVA (Most Valuable Victim - Least Valuable Aggressor) capture ordering
- **Killer Move Heuristic**: Non-capture move ordering optimization
- **Piece-Square Tables**: Positional evaluation for all piece types with separate endgame tables
- **Material Evaluation**: Standard piece values with bishop pair consideration

### Technical Features
- **Cross-Platform**: Compatible with Windows, macOS, and Linux
- **CPU Intrinsics**: Optimized bit manipulation using platform-specific instructions
- **Time Management**: Intelligent time allocation with configurable move overhead
- **Perft Testing**: Move generation verification and debugging utility

## Building from Source

### Prerequisites
- C++20 compatible compiler (GCC, Clang, or MSVC)
- Make utility

### Compilation

```bash
make
```

This will compile the engine and produce the executable `nice.exe`.

### Clean Build

```bash
make clean
make
```

## Usage

### Running with a UCI GUI

1. Build the engine following the instructions above
2. Open your preferred UCI-compatible chess GUI
3. Add NICE as an engine by pointing to the compiled executable
4. Configure time controls and play

### Command Line Interface

Run the engine directly:

```bash
./nice.exe
```

The engine communicates via UCI protocol. Common UCI commands:

```
uci                          # Initialize UCI mode
isready                      # Check if engine is ready
ucinewgame                   # Start a new game
position startpos            # Set starting position
position fen [FEN string]    # Set position from FEN
go movetime [ms]             # Search with fixed time
go wtime [ms] btime [ms]     # Search with clock times
quit                         # Exit engine
```

### Example Session

```
> uci
id name NICE 1.0
id author Tarique Mackay
uciok

> isready
readyok

> position startpos moves e2e4 e7e5
> go movetime 5000
bestmove g1f3

> quit
```

## Project Structure

```
src/
├── BitUtils.hpp      # Bit manipulation utilities and intrinsics
├── Board.cpp/hpp     # Board representation and move execution
├── Evaluation.cpp/hpp# Position evaluation with PST
├── Move.hpp          # Move encoding and decoding
├── MoveGen.cpp/hpp   # Legal move generation
├── Perft.cpp/hpp     # Move generation testing
├── Search.cpp/hpp    # Search algorithm implementation
├── Types.hpp         # Type definitions and constants
├── UCI.cpp/hpp       # UCI protocol handler
└── main.cpp          # Entry point
```

## Technical Details

### Move Encoding
Moves are encoded in 32-bit integers with the following layout:
- Bits 0-5: From square (0-63)
- Bits 6-11: To square (0-63)
- Bits 12-17: Move flags (capture, promotion, castling, etc.)
- Bits 18-21: Promotion piece type
- Bits 22-25: Captured piece type

### Bitboard Representation
The engine uses 16 bitboards to represent the position:
- 12 piece bitboards (6 white pieces, 6 black pieces)
- 3 occupancy bitboards (white, black, all)
- 1 reserved

### Search Features
- Negamax framework with fail-soft alpha-beta pruning
- Quiescence search for tactical stability
- Killer move heuristic (2 killer moves per ply)
- MVV-LVA move ordering for captures
- Time management with safety buffer
- Iterative deepening up to depth 64

### Evaluation Components
- Material counting with standard values (P=100, N=300, B=350, R=500, Q=900)
- Piece-square tables for positional evaluation
- Separate king tables for middlegame and endgame
- Perspective-based score return (positive for side to move)

## Performance Testing

The engine includes Perft (performance test) functionality for move generation verification:

```cpp
Board board;
Perft::perftDivide(board, 5);  // Run perft to depth 5
```

## Future Enhancements

- Transposition tables for position caching
- Opening book integration
- Endgame tablebases support
- Multi-threaded search
- Advanced pruning techniques (null move, futility pruning)
- Tunable evaluation parameters

## License

This project is open source. See source files for specific licensing information.

## Acknowledgments

Built using modern chess programming techniques and inspired by the chess programming community's extensive knowledge base.
