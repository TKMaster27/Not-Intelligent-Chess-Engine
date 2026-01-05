#include "Evaluation.hpp"
#include "BitUtils.hpp"
#include "Types.hpp"

int Evaluation::evaluate(Board &board) {
    int score = 0;

    // --- white evaluation --

    // determine if there is no queen to decide which PST kings should use
    const int* whiteKingTableToUse = (board.bitboards[BQ] == 0) ? kingEndgameTable : kingTable;

    // king points
    U64 bitboard = board.bitboards[WK];
    while (bitboard) {
        int piecePosition = popLSB(bitboard);
        score += pieceValues[WK] + whiteKingTableToUse[piecePosition^56]; // material score + PST score
    }

    // pawn points
    bitboard = board.bitboards[WP];
    while (bitboard) {
        int piecePosition = popLSB(bitboard);
        score += pieceValues[WP] + pawnTable[piecePosition^56]; // material score + PST score
    }

    // knight points
    bitboard = board.bitboards[WN];
    while (bitboard) {
        int piecePosition = popLSB(bitboard);
        score += pieceValues[WN] + knightTable[piecePosition^56]; // material score + PST score
    }

    // bishop points
    bitboard = board.bitboards[WB];
    while (bitboard) {
        int piecePosition = popLSB(bitboard);
        score += pieceValues[WB] + bishopTable[piecePosition^56]; // material score + PST score
    }
    
    // rooks points
    bitboard = board.bitboards[WR];
    while (bitboard) {
        int piecePosition = popLSB(bitboard);
        score += pieceValues[WR] + rookTable[piecePosition^56]; // material score + PST score
    }

    // queen points
    bitboard = board.bitboards[WQ];
    while (bitboard) {
        int piecePosition = popLSB(bitboard);
        score += pieceValues[WQ] + queenTable[piecePosition^56]; // material score + PST score
    }

    // --- black evaluation --

    // determine if there is no queen to decide which PST kings should use
    const int* blackKingTableToUse = (board.bitboards[WQ] == 0) ? kingEndgameTable : kingTable;

        // king points
    bitboard = board.bitboards[BK];
    while (bitboard) {
        int piecePosition = popLSB(bitboard);
        score -= pieceValues[BK-6] + blackKingTableToUse[piecePosition]; // material score + PST score
    }

    // pawn points
    bitboard = board.bitboards[BP];
    while (bitboard) {
        int piecePosition = popLSB(bitboard);
        score -= pieceValues[BP-6] + pawnTable[piecePosition]; // material score + PST score
    }

    // knight points
    bitboard = board.bitboards[BN];
    while (bitboard) {
        int piecePosition = popLSB(bitboard);
        score -= pieceValues[BN-6] + knightTable[piecePosition]; // material score + PST score
    }

    // bishop points
    bitboard = board.bitboards[BB];
    while (bitboard) {
        int piecePosition = popLSB(bitboard);
        score -= pieceValues[BB-6] + bishopTable[piecePosition]; // material score + PST score
    }
    
    // rooks points
    bitboard = board.bitboards[BR];
    while (bitboard) {
        int piecePosition = popLSB(bitboard);
        score -= pieceValues[BR-6] + rookTable[piecePosition]; // material score + PST score
    }

    // queen points
    bitboard = board.bitboards[BQ];
    while (bitboard) {
        int piecePosition = popLSB(bitboard);
        score -= pieceValues[BQ-6] + queenTable[piecePosition]; // material score + PST score
    }

    // return positive score for white and negative for black
    return (board.activeColour == WHITE) ? score : -score;
}