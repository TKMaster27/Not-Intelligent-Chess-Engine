#include "Evaluation.hpp"
#include "BitUtils.hpp"
#include "Types.hpp"

int Evaluation::evaluate(Board &board) {
    int score = 0;

    // sum up material score for white
    for (int piece = WP; piece <= WK; piece++){
        int pieceCount = popCount(board.bitboards[piece]);
        score += pieceCount * pieceValues[piece];
    }

    // subtract sum up material score for black
    for (int piece = BP; piece <= BK; piece++){
        int pieceCount = popCount(board.bitboards[piece]);
        score -= pieceCount * pieceValues[piece];
    }

    return (board.activeColour == WHITE) ? score : -score;
}