#ifndef _BITUTILS_H
#define _BITUTILS_H

#include <cstdint>
#if defined(_MSC_VER) // for windows and cross compatability :(
#include <intrin.h> // for linux/macos cpu bit manipulation commands
#pragma intrinsic(_BitScanForward64)
#endif

typedef uint64_t U64;

// gets the least signigican bit index by counting trailing zeros (cpu instruction)
inline int getLSB(U64 bitboard){
    #if defined(_MSC_VER)
        unsigned long index;
        _BitScanForward64(&index, bboard); // find index with windows
        return index;
    # else
        return __builtin_ctzll(bitboard); // count trailing zeros with gcc/clang
    # endif
}

// get LSB and pop it from bitboard
inline int popLSB(U64 &bitboard){
    int lsb = getLSB(bitboard); // get lsb
    bitboard &= bitboard-1; // clear lsb
    return lsb;
}

// check if a bit is set
inline bool getBit(U64 bitboard, int square){
    return (bitboard & (1ULL << square));
}

// set the bit given square
inline void setBit(U64 &bitboard, int square) {
    bitboard |= (1ULL << square);
}

#endif