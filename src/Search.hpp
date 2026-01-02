#ifndef _SEARCH_H
#define _SEARCH_H

#include "Board.hpp"
#include "Move.hpp"

class Search {
    public: 
        static Move searchPosition(const Board &board, int depth);

    private:
        static int negamax(Board &board, int alpha, int beta, int depth);
        static int quiescence(Board &booard, int alpha, int beta);
};

#endif 