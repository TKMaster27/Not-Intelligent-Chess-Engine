#ifndef CHESS_EVALUATION_HPP
#define CHESS_EVALUATION_HPP

#include "Board.hpp"


class Evaluation {
    public:
        // returns a score for the given board state
        // postive is good for white, negative is good for black
        static int evaluate(Board &board);
};

#endif