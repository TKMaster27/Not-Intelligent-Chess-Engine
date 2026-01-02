#include <iostream>
#include <string>
#include "Board.hpp"
#include "Move.hpp"
#include "Types.hpp"
#include "MoveGen.hpp"
#include "Perft.hpp"
#include "UCI.hpp"

int main(int argc, char* argv[]) {

    if (argc == 1) {
      UCI::loop();
      return 0;
    }
    
    // Default to Start Position if no args provided (for quick testing)
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    int depth = 1;

    // optional commands for testing
    // Usage: ./engine "FEN" depth
    if (argc >= 3) {
        fen = argv[1];
        depth = std::stoi(argv[2]);
    }

    Board board(fen);
    
    // Run the divide function (shows detail)
    uint64_t result = Perft::perft(board, depth);
    // board.printBoard();
    // Perft::perftDivide(board, depth);
    

    // Output ONLY the result
    std::cout << result << std::endl;

    return 0;
}