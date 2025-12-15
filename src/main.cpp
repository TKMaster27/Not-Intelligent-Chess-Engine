#include <iostream>
#include <string>
#include "Board.hpp"
#include "Move.hpp"
#include "Types.hpp"
#include "MoveGen.hpp"
#include "Perft.hpp"

int main(int argc, char* argv[]) {
    
    // Default to Start Position if no args provided (for quick testing)
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    int depth = 1;

    // Optional: Parse command line args for automated testing
    // Usage: ./engine "FEN" depth
    if (argc >= 3) {
        fen = argv[1];
        depth = std::stoi(argv[2]);
    }

    std::cout << "Running Perft Test..." << std::endl;
    std::cout << "FEN:   " << fen << std::endl;
    std::cout << "Depth: " << depth << std::endl;

    Board board(fen);
    
    // Run the divide function (shows detail)
    Perft::perftDivide(board, depth);

    return 0;
}