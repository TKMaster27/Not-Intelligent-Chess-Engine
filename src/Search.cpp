#include "Search.hpp"
#include "Evaluation.hpp"
#include "MoveGen.hpp"
#include "BitUtils.hpp"
#include <iostream>

#define MATE_VALUE 49000
#define INVALID_SCORE -200000

int Search::negamax(Board &board, int depth){

    // base case
    if(depth == 0){
        return Evaluation::evaluate(board);
    }

    // generate all possible moves
    std::vector<Move> moves = MoveGen::generateMoves(board);

    int bestScore = INVALID_SCORE;
    int legalMoves = 0;

    for (const Move &move: moves){
        Board nextBoard = board; // copy current board
        nextBoard.makeMove(move); // make move from movelist

        // get position of the king of the current board
        int kingType = (board.activeColour == WHITE) ? WK : BK;
        int kingSquare = getLSB(nextBoard.bitboards[kingType]);

        // check if the king of the current board is under attack on the next board by the next boards active colour
        if(MoveGen::isSquareAttacked(nextBoard, kingSquare, nextBoard.activeColour)){
            continue; // skip because board is illegal for the current board player
        }

        legalMoves++;

        // recursive step - get score of the board after move is made
        int score = -negamax(nextBoard, depth-1);

        // get max score
        if(score > bestScore){
            bestScore = score;
        }
    }

    // evaluate checkmate and stale mate positions to make checkmate desireable and invalid for stalemate

    if(legalMoves == 0){

        // get position of the king of the current board
        int kingType = (board.activeColour == WHITE) ? WK : BK;
        int kingSquare = getLSB(board.bitboards[kingType]);

        // getting opponents colour
        int attacker = (board.activeColour == WHITE) ? BLACK : WHITE;

        if(MoveGen::isSquareAttacked(board, kingSquare, attacker)){
            return -MATE_VALUE + depth; // try to mate sooner
        } else {
            return 0;
        }
    }

    return bestScore;
}

// wrapper for negamax and keep track of the best move associated with the best score
Move Search::searchPosition(const Board &board, int depth){

    // generate all possible moves
    std::vector<Move> moves = MoveGen::generateMoves(board);

    Move bestMove = 0;
    int bestScore = INVALID_SCORE;

    for (const Move &move: moves){
        Board nextBoard = board; // copy current board
        nextBoard.makeMove(move); // make move from movelist

        // get position of the king of the current board
        int kingType = (board.activeColour == WHITE) ? WK : BK;
        int kingSquare = getLSB(nextBoard.bitboards[kingType]);

        // check if the king of the current board is under attack on the next board by the next boards active colour
        if(MoveGen::isSquareAttacked(nextBoard, kingSquare, nextBoard.activeColour)){
            continue; // skip because board is illegal for the current board player
        }


        // recursive step - get score of the board after move is made
        int score = -negamax(nextBoard, depth-1);

        // uci info about search
        std::cout << "info score cp " << score
                  << " pv "
                  << board.convertSquareToCord(fromSq(move))
                  << board.convertSquareToCord(toSq(move))
                  << std::endl;


        // get max score
        if(score > bestScore){
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove;
}

