#ifndef _EVALUATION_H
#define _EVALUATION_H

#include "Board.hpp"


class Evaluation {
    public:
        // returns a score for the given board state
        // postive is good for white, negative is good for black
        static int evaluate(Board &board);
};

#endif