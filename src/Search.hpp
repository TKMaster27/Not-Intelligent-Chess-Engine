#ifndef _SEARCH_H
#define _SEARCH_H

#include "Board.hpp"
#include "Move.hpp"

class Search {
    public: 
        static Move searchPosition(const Board &board, int depth);

    private:
        static int negamax(Board &board, int depth);
};

#endif 