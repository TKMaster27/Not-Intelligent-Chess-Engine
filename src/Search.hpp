#ifndef CHESS_SEARCH_HPP
#define CHESS_SEARCH_HPP

#include "Board.hpp"
#include "Move.hpp"

class Search {
    public: 
        static Move searchPosition(const Board &board, int depth);

    private:
        static int negamax(Board &board, int alpha, int beta, int depth);
        static int quiescence(Board &booard, int alpha, int beta);
        static int scoreMove(const Move &move);
};

#endif 