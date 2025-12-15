#ifndef _PERFT_H
#define _PERFT_H

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