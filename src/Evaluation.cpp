#include "Evaluation.hpp"
#include "BitUtils.hpp"
#include "Types.hpp"

int Evaluation::evaluate(Board &board) {
    int score = 0;
    int pieceCount = 0;

    // --- white evaluation --

    // pawn points
    U64 bitboard = board.bitboards[WP];
    while (bitboard) {
        int piecePosition = popLSB(bitboard);
        score += pieceValues[WP] + pawnTable[piecePosition]; // material score + PST score
    }

    // knight points
    bitboard = board.bitboards[WN];
    while (bitboard) {
        int piecePosition = popLSB(bitboard);
        score += pieceValues[WN] + knightTable[piecePosition]; // material score + PST score
    }

    // king points
    bitboard = board.bitboards[WK];
    while (bitboard) {
        int piecePosition = popLSB(bitboard);
        score += pieceValues[WK] + kingTable[piecePosition]; // material score + PST score
    }

    // sum up material score for white bishops
    pieceCount = popCount(board.bitboards[WB]);
    score += pieceCount * pieceValues[WB];

    // sum up material score for white bishops
    pieceCount = popCount(board.bitboards[WR]);
    score += pieceCount * pieceValues[WR];

    // sum up material score for white queens
    pieceCount = popCount(board.bitboards[WQ]);
    score += pieceCount * pieceValues[WQ];
    

    // --- black evaluation --

    // pawn points
    bitboard = board.bitboards[BP];
    while (bitboard) {
        int piecePosition = popLSB(bitboard);
        score -= pieceValues[BP-6] + pawnTable[piecePosition^56]; // material score + PST score
    }

    // knight points
    bitboard = board.bitboards[BN];
    while (bitboard) {
        int piecePosition = popLSB(bitboard);
        score -= pieceValues[BN-6] + knightTable[piecePosition^56]; // material score + PST score
    }

    // king points
    bitboard = board.bitboards[BK];
    while (bitboard) {
        int piecePosition = popLSB(bitboard);
        score -= pieceValues[BK-6] + kingTable[piecePosition^56]; // material score + PST score
    }

    // sum up material score for black bishops
    pieceCount = popCount(board.bitboards[BB]);
    score -= pieceCount * pieceValues[BB-6];

    // sum up material score for white bishops
    pieceCount = popCount(board.bitboards[BR]);
    score += pieceCount * pieceValues[BR-6];

    // sum up material score for black queens
    pieceCount = popCount(board.bitboards[BQ]);
    score -= pieceCount * pieceValues[BQ-6];

    // return positive score for white and negative for black
    return (board.activeColour == WHITE) ? score : -score;
}