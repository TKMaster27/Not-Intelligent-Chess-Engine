#ifndef CHESS_PERFT_HPP
#define CHESS_PERFT_HPP

#include "Board.hpp"
#include <cstdint>

class Perft {
    public:

    // counts nodes/positions recursively
    static uint64_t perft(Board& board, int depth);

    // prints the node count to console
    static void perftDivide(Board& board, int depth);
};

#endif