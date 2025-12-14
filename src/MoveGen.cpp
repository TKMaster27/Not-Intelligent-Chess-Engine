#include "MoveGen.hpp"
#include "BitUtils.hpp"
#include "Types.hpp"

// static lookup tables for king and knight attacks
// table will include attacking bitmask for every square
static U64 knightAttacks[64];
static U64 kingAttacks[64];
static bool isInitialised = false;

// helper function to calculate knight attacking positions for a given square
static U64 calculateKnightAttacks(int square) {
    U64 attacks = 0;
    U64 bitboard = 0;

    // set the given square where the knight is
    setBit(bitboard, square);

    const U64 notA  = 0xFEFEFEFEFEFEFEFE; // ~File A
    const U64 notB  = 0xFDFDFDFDFDFDFDFD; // ~File B 
    const U64 notH  = 0x7F7F7F7F7F7F7F7F; // ~File H
    const U64 notG  = 0xBFBFBFBFBFBFBFBF; // ~File G 
    
    const U64 notAB = 0xFCFCFCFCFCFCFCFC; // ~File A & ~File B
    const U64 notGH = 0x3F3F3F3F3F3F3F3F; // ~File G & ~File H

    attacks |= ((bitboard & notA) >> 17); // check if SSE is on board
    attacks |= ((bitboard & notH) >> 15); // check if SSW is on board
    attacks |= ((bitboard & notAB) >> 10); // check if SEE is on board
    attacks |= ((bitboard & notGH) >> 6); // check if SWW is on board
    
    attacks |= ((bitboard & notH) << 17); // check if NNW is on board
    attacks |= ((bitboard & notA) << 15); // check if NNE is on board
    attacks |= ((bitboard & notGH) << 10); // check if NWW is on board
    attacks |= ((bitboard & notAB) << 6); // check if NEE is on board

    return attacks;
}


// helper function to calculate king attacking positions for a given square
static U64 calculateKingAttacks(int square) {
    U64 attacks = 0;
    U64 bitboard = 0;

    // set the given square where the knight is
    setBit(bitboard, square);

    U64 clipFileA = 0xFEFEFEFEFEFEFEFE;
    U64 clipFileH = 0x7F7F7F7F7F7F7F7F;

    // North, South
    attacks |= (bitboard << 8); 
    attacks |= (bitboard >> 8); 
    
    // East, West
    attacks |= (bitboard << 1) & clipFileA;
    attacks |= (bitboard >> 1) & clipFileH;
    
    // Diagonals
    attacks |= (bitboard << 9) & clipFileA;
    attacks |= (bitboard >> 9) & clipFileH;
    attacks |= (bitboard << 7) & clipFileH;
    attacks |= (bitboard >> 7) & clipFileA;

    return attacks;
}

// Initialize tables once
static void initTables() {
    for (int sq = 0; sq < 64; sq++) {
        knightAttacks[sq] = calculateKnightAttacks(sq);
        kingAttacks[sq]   = calculateKingAttacks(sq);
    }
    isInitialised = true;
}

// move generator

std::vector<Move> MoveGen::generateMoves(const Board &board){

    // if king/knight attack tables have not been initialised
    if (!isInitialised) initTables();

    std::vector<Move> MoveList;
    MoveList.reserve(256); // reserve max moves to not need to resize vector

    generateKingMoves(board, MoveList);
    generateKnightMoves(board, MoveList);
    // generatePawnMoves(board, MoveList);
    generateSlidingMoves(board, MoveList);

    return MoveList;
}

void MoveGen::generateKnightMoves(const Board &board, std::vector<Move> &moveList){
    int side = board.activeColour;
    int enemy = (side == WHITE) ? BLACK : WHITE;
    
    int knightType = (side == WHITE) ? WN : BN;
    U64 knights = board.bitboards[knightType];

    // occupancy masks
    U64 sameColourPieces = (side == WHITE) ? board.bitboards[WHITE_OCC] : board.bitboards[BLACK_OCC];
    U64 enemyPieces = (side == WHITE) ? board.bitboards[BLACK_OCC] : board.bitboards[WHITE_OCC];

    while (knights) { // there are knights on the board to evaluate

        int from = popLSB(knights); // get location of knight

        // lookup attack for that square
        U64 attacks = knightAttacks[from];

        // remove places of friendly pices (can't caputre own pieces)
        attacks &= ~sameColourPieces;

        // validate attacks
        while (attacks){
            int to = popLSB(attacks);

            if (getBit(enemyPieces, to)){ // if we land on an enemy piece
                int capturedPiece = board.boardArr[to]; // identify captured piece
                moveList.push_back(makeMove(from, to, CAPTURE, 0, capturedPiece)); // show capture in move
            } else {
                moveList.push_back(makeMove(from, to));
            }
        }
    }
}

void MoveGen::generateKingMoves(const Board &board, std::vector<Move> &moveList){
    int side = board.activeColour;
    
    int kingType = (side == WHITE) ? WK : BK;
    U64 king = board.bitboards[kingType];

    // occupancy masks
    U64 sameColourPieces = (side == WHITE) ? board.bitboards[WHITE_OCC] : board.bitboards[BLACK_OCC];
    U64 enemyPieces = (side == WHITE) ? board.bitboards[BLACK_OCC] : board.bitboards[WHITE_OCC];

    while (king) { // there are knights on the board to evaluate

        int from = popLSB(king); // get location of knight

        // lookup attack for that square
        U64 attacks = kingAttacks[from];

        // remove places of friendly pices (can't caputre own pieces)
        attacks &= ~sameColourPieces;

        // validate attacks
        while (attacks){
            int to = popLSB(attacks);

            if (getBit(enemyPieces, to)){ // if we land on an enemy piece
                int capturedPiece = board.boardArr[to]; // identify captured piece
                moveList.push_back(makeMove(from, to, CAPTURE, 0, capturedPiece)); // show capture in move
            } else {
                moveList.push_back(makeMove(from, to));
            }
        }
    }
}

void MoveGen::generateSlidingMoves(const Board &board, std::vector<Move> &moveList){
    int side = board.activeColour;

    // get occupancy bitboards
    U64 sameColourPieces = (side == WHITE) ? board.bitboards[WHITE_OCC] : board.bitboards[BLACK_OCC];
    U64 enemyPieces = (side == WHITE) ? board.bitboards[BLACK_OCC] : board.bitboards[WHITE_OCC]; 

    // we will loop through each sliding piece (Bishop=2, Rook=3, Queen=4)
    int pStart = (side == WHITE) ? WB : BB;
    int pEnd = (side == WHITE) ? WQ : BQ;

    for(int pieceType = pStart; pieceType <= pEnd; ++pieceType){

        // get bitboard/location of all pieces of current type
        U64 bitboard = board.bitboards[pieceType];

        while(bitboard){
            int from = popLSB(bitboard);

            // direction arrays for each piece (which way they move)
            // arrays are static so they are not recreated each time loop runs and created once
            static const int rookDirections[] = {NORTH, EAST, SOUTH, WEST};
            static const int bishopDirections[] = {NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORHT_WEST};
            static const int queenDirections[] = {NORTH, NORTH_EAST, EAST, SOUTH_EAST, SOUTH, SOUTH_WEST, WEST, NORHT_WEST};

            // pointer to the directions of current piece (naked pointer because now ownership of resources)
            const int* offsets = nullptr;
            int directionSize = 0;

            // assign offset to current piece type

            if(pieceType == WR || pieceType == BR){
                offsets = rookDirections;
                directionSize=4;
            } else if (pieceType == WB || pieceType == BB) {
                offsets = bishopDirections;
                directionSize = 4;
            } else {
                offsets = queenDirections;
                directionSize = 8;
            }

            for (int i = 0; i < directionSize; ++i){ // for each direction based on offsets and piece
                int offset = offsets[i];
                int to = from;

                while(true) { // take a step in that direction

                    // stop if moved too far to the left off the board (stop at A file)
                    if ((offset == WEST || offset == NORHT_WEST || offset == SOUTH_WEST) && (to % 8) == 0) {
                        break;
                    }

                    // stop if moved too far to the right off the board (stop at H file)
                    if ((offset == EAST || offset == NORTH_EAST || offset == SOUTH_EAST) && (to % 8) == 7) {
                        break;
                    }

                    to += offset;

                    if (to < 0 || to > 63) break; // out of bound of the board

                    // if friendly piece found, stop
                    if(getBit(sameColourPieces, to)){
                        break; // stop seach
                    }

                    // if enemy peice found
                    if(getBit(enemyPieces, to)) {
                        int capturedPiece = board.boardArr[to];
                        moveList.push_back(makeMove(from, to, CAPTURE, 0, capturedPiece));
                        break; // stop seaching after capture
                    }

                    // otherwise quiet move
                    moveList.push_back(makeMove(from, to));

                }
            }
        }


    }
}