
#ifndef CHESS_MOVE_HPP
#define CHESS_MOVE_HPP

#include <cstdint>

using Move = uint32_t;

enum MoveFlags: uint8_t {
    QUIET           = 0,
    CAPTURE         = 1 << 0,
    DOUBLE_PUSH     = 1 << 1,
    EN_PASSANT      = 1 << 2,
    CASTLING        = 1 << 3,
    PROMOTION       = 1 << 4,
};

/*
Move encoding explained FFFFFFSSSSSSXXX
bits 0-5        : from square (0-63), 6 bits needed
bits 6-11       : to square (0-63), 6 bits needed
bits 12-17      : flags       (6 bits)  
bits 18-21      : promo piece (4 bits)  
bits 22-25      : captured    (4 bits)  
*/

inline Move makeMove(int from, int to, int flags=0, int promo=0, int captured=0) {
    return from | (to << 6) |  (flags << 12) | (promo << 18) | (captured << 22); 
}

// 0x3F is a bitmask for the last 6 bits
// 0xF is a bitmask for the last 3 bits
inline int fromSq(Move m) {return m             & 0x3F;}
inline int toSq(Move m) {return m >> 6          & 0x3F;} 
inline int moveFlags(Move m) {return m >> 12    & 0x3F;} 
inline int promo(Move m) {return m >> 18        & 0xF;} 
inline int captured(Move m) {return m >> 22     & 0xF;} 


#endif