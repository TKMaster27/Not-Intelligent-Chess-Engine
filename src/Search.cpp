#include "Search.hpp"
#include "Evaluation.hpp"
#include "MoveGen.hpp"
#include "BitUtils.hpp"
#include <iostream>

#define MATE_VALUE 49000
#define INVALID_SCORE -200000

// scores moves to ensure move order and maximum pruning
int Search::scoreMove(const Move &move){
    // prioritise captures with the MVV-LVA methodology (Most Valuable Victum - Least Valuable Agressor)
    if(moveFlags(move) & CAPTURE){
        int victimType = captured(move);

        int vitimValue = pieceValues[victimType%6]; // % 6 for both black and white piece types

        return vitimValue + 10000;
    }

    // bonuses for promotion in case for queen
    if(moveFlags(move) & PROMOTION){
        int promotionType = promo(move);
        return 5000 + pieceValues[promotionType%6];
    }

    return 0;
}

// searches deeper when captures are discovered on leaf nodes of search
int Search::quiescence(Board &board, int alpha, int beta){
    int eval = Evaluation::evaluate(board);

    // fail beta cutoff, prune
    if (eval >= beta) {
        return beta;
    }

    // found beta score
    if(eval > alpha){
        alpha = eval;
    }

    // generate all possible moves
    std::vector<Move> moves = MoveGen::generateMoves(board);

    //sort moves for maximum pruning
    std::sort(moves.begin(), moves.end(), [&](const Move &a, const Move &b) {return scoreMove(a) > scoreMove(b);});

    for (const Move &move: moves){

        //check if move has capture, only extendsearch for captures
        if(!(moveFlags(move) & CAPTURE)){
            continue;
        }

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
        int score = -quiescence(nextBoard, -beta, -alpha);

        // fail beta cutoff, prune
        if (score >= beta) {
            return beta;
        }

        // found beta score
        if(score > alpha){
            alpha = score;
        }
    }

    return alpha;
}

int Search::negamax(Board &board, int alpha, int beta, int depth){

    // base case
    if(depth == 0){
        return quiescence(board, alpha, beta);
    }

    // generate all possible moves
    std::vector<Move> moves = MoveGen::generateMoves(board);

    //sort moves for maximum pruning
    std::sort(moves.begin(), moves.end(), [&](const Move &a, const Move &b) {return scoreMove(a) > scoreMove(b);});

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
        int score = -negamax(nextBoard, -beta, -alpha, depth-1);

        // fail beta cutoff, prune
        if (score >= beta) {
            return beta;
        }

        // found beta score
        if(score > alpha){
            alpha = score;
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
            return -MATE_VALUE - depth; // try to mate sooner
        } else {
            return 0;
        }
    }

    return alpha;
}

// wrapper for negamax and keep track of the best move associated with the best score
Move Search::searchPosition(const Board &board, int depth){

    // generate all possible moves
    std::vector<Move> moves = MoveGen::generateMoves(board);

    //sort moves for maximum pruning
    std::sort(moves.begin(), moves.end(), [&](const Move &a, const Move &b) {return scoreMove(a) > scoreMove(b);});

    Move bestMove = 0;

    // Initial bounds for Alpha-Beta
    int alpha = INVALID_SCORE; 
    int beta = -INVALID_SCORE;  


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
        int score = -negamax(nextBoard, -beta, -alpha, depth-1);

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

        if(score > alpha){
            alpha = score;
        }
    }

    return bestMove;
}

