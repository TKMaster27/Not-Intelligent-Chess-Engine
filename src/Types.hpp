#ifndef _TYPES_H
#define _TYPES_H

#include <cstdint>

// Bitboard type alias (makes code easier to read)
typedef uint64_t U64;

enum Pieces{
    // White Pieces
    WP, WN, WB, WR, WQ, WK, // 0 - 5

    // Black Pieces
    BP, BN, BB, BR, BQ, BK,  // 6 - 11

    NO_PIECE = 15 // last entry in bitboard
};

// Bitboard/Occupancy indices
enum Occupancy {
    WHITE_OCC = 12,
    BLACK_OCC = 13,
    ALL_OCC  = 14
};

// Colors
enum Color {
    WHITE, BLACK, BOTH
};

// Squares (0 = a1, 63 = h8)
enum Square {
    SQ_A1 = 0, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
    SQ_A2 = 8, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
    SQ_A3 = 16, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
    SQ_A4 = 24, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
    SQ_A5 = 32, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
    SQ_A6 = 40, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
    SQ_A7 = 48,  SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
    SQ_A8 = 56,  SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
    NO_SQ = -1
};

// Castling Rights (Bitmask)
enum CastlingRights {
    WK_CA = 1, // 0001
    WQ_CA = 2, // 0010
    BK_CA = 4, // 0100
    BQ_CA = 8  // 1000
};

enum Directions {
    NORTH      =  8,
    EAST       =  1,
    SOUTH      = -8,
    WEST       = -1,

    NORTH_EAST =  9, 
    NORTH_WEST =  7, 
    SOUTH_EAST = -7,
    SOUTH_WEST = -9  
};

// values in order pawn, knight, bishop, rook, queen, king
// standard piece values times 100 with a bishop weighinh slightly better than kight
constexpr int pieceValues[] = {100, 300, 350, 500, 900, 100000};


#endif