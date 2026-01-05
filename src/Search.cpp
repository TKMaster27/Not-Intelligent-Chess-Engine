#include "Search.hpp"
#include "Evaluation.hpp"
#include "MoveGen.hpp"
#include "BitUtils.hpp"
#include <iostream>
#include <algorithm>
#include <chrono>

#define MATE_VALUE 49000
#define INVALID_SCORE -200000

// time managment global variables
static std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
static int allocatedTime = 0;
static bool stopSearch = false;
static long nodesVisited = 0;

Move killerMoves[64][2];

// scores moves to ensure move order and maximum pruning
int Search::scoreMove(const Move &move, int ply){
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

    // bonus points if the current move is a killer move
    if (move == killerMoves[ply][0]) return 9000;
    if (move == killerMoves[ply][1]) return 8000;

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
    std::sort(moves.begin(), moves.end(), [&](const Move &a, const Move &b) {return scoreMove(a, 0) > scoreMove(b, 0);});

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

int Search::searchRoot(Board &board, int alpha, int beta, int depth, Move &bestMoveOut){

    // generate all possible moves
    std::vector<Move> moves = MoveGen::generateMoves(board);

    //sort moves for maximum pruning
    std::sort(moves.begin(), moves.end(), [&](const Move &a, const Move &b) {return scoreMove(a, 0) > scoreMove(b, 0);});

    int bestScore = -50000;
    Move bestLocalMove = 0;
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
        int score = -negamax(nextBoard, -beta, -alpha, depth-1, 1);

        if (stopSearch) return 0;

        // update best move at end of search and time
        if (score > bestScore) {
            bestScore = score;
            bestLocalMove = move;
        }

        // fail beta cutoff, prune
        if (score >= beta) {
            break;
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

    if(!stopSearch){
        bestMoveOut = bestLocalMove;
    }

    return bestScore;
}

int Search::negamax(Board &board, int alpha, int beta, int depth, int ply){

    // --- start time check ---
    // every 2048 nodes check the time (same as nodesVis % 2048)
    if ((nodesVisited++ & 2047) == 0) {
        auto now = std::chrono::high_resolution_clock::now();
        // duration from now and start time
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now-startTime).count();

        if(duration >= allocatedTime){
            stopSearch = true;
        }
    }

    // if ran out of time, break search
    if (stopSearch) return 0;

    // --- end time check


    // base case
    if(depth == 0){
        return quiescence(board, alpha, beta);
    }

    // generate all possible moves
    std::vector<Move> moves = MoveGen::generateMoves(board);

    //sort moves for maximum pruning
    std::sort(moves.begin(), moves.end(), [&](const Move &a, const Move &b) {return scoreMove(a, ply) > scoreMove(b, ply);});

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
        int score = -negamax(nextBoard, -beta, -alpha, depth-1, ply+1);

        // fail beta cutoff, prune
        if (score >= beta) {

            if ( !(moveFlags(move) & CAPTURE) ) {
                // Shift old killer to slot 1
                killerMoves[ply][1] = killerMoves[ply][0];
                // Store new killer in slot 0
                killerMoves[ply][0] = move;
            }

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
Move Search::searchPosition(const Board &board, int timeInMilliSec){
    // local copy for const correctness
    Board searchBoard = board;

    // clear killer moves
    for(int i = 0; i < 64; i++){
        killerMoves[i][0] = 0;
        killerMoves[i][1] = 0;
    }

    // initialise time management variables
    stopSearch = false;
    nodesVisited = 0;
    startTime = std::chrono::high_resolution_clock::now();

    // allocate time with small buffer for breaking out of search
    allocatedTime = timeInMilliSec-50;
    if (allocatedTime < 50) allocatedTime = 50; // minmum time for search 

    Move bestMove = 0;
    
    // Initial bounds for Alpha-Beta
    int alpha = INVALID_SCORE; 
    int beta = -INVALID_SCORE;  

    // iterative deepening step
    for (int currentDepth = 1; currentDepth <= 64; currentDepth++){
        Move bestMoveInIteration = 0;

        int score = searchRoot(searchBoard, alpha, beta, currentDepth, bestMoveInIteration);

        // check timeout

        if(stopSearch){
            // search not complete, do not save result
            break;
        }

        // search completed before timeout, save result

        bestMove = bestMoveInIteration;

        // uci info about search
        std::cout << "info depth " << currentDepth 
                  << " score cp " << score 
                  << " nodes " << nodesVisited
                  << " pv " 
                  << board.convertSquareToCord(fromSq(bestMove)) 
                  << board.convertSquareToCord(toSq(bestMove)) 
                  << std::endl;

        // early stop for forced mate
        if (score > 48000 || score < -48000) break;

    }

    return bestMove;
}

