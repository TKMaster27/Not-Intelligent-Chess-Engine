#include <iostream>
#include <string>
#include "Board.hpp"
#include "Move.hpp"
#include "Types.hpp"
#include "MoveGen.hpp"
#include "Perft.hpp"

int main(int argc, char* argv[]) {
    
    // Default to Start Position if no args provided (for quick testing)
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    int depth = 1;

    // Optional: Parse command line args for automated testing
    // Usage: ./engine "FEN" depth
    if (argc >= 3) {
        fen = argv[1];
        depth = std::stoi(argv[2]);
    }

    Board board(fen);
    
    // Run the divide function (shows detail)
    uint64_t result = Perft::perft(board, depth);
    // board.printBoard();
    // Perft::perftDivide(board, 1);
    

    // Output ONLY the result
    std::cout << result << std::endl;

    return 0;
}