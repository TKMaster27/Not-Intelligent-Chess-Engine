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
    generatePawnMoves(board, MoveList);
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
            static const int bishopDirections[] = {NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST};
            static const int queenDirections[] = {NORTH, NORTH_EAST, EAST, SOUTH_EAST, SOUTH, SOUTH_WEST, WEST, NORTH_WEST};

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
                    if ((offset == WEST || offset == NORTH_WEST || offset == SOUTH_WEST) && (to % 8) == 0) {
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

void MoveGen::generatePawnMoves(const Board &board, std::vector<Move> &moveList){

    int side = board.activeColour;
    
    int pawnType = (side == WHITE) ? WP : BP;
    U64 pawns = board.bitboards[pawnType];

    // occupancy masks
    U64 enemyPieces = (side == WHITE) ? board.bitboards[BLACK_OCC] : board.bitboards[WHITE_OCC];    
    U64 occupiedSquares = board.bitboards[ALL_OCC];

    // White Pawns (travel up the board) NORTH

    if (side == WHITE){

        // while there are pawns on the board
        while(pawns) {
            int from = popLSB(pawns);
            int to;

            // single pawn push

            to = from + NORTH; // move one square north

            if (to >= SQ_A8 && !getBit(occupiedSquares, to)){ // if still on the board and no occupied square in front of pawn

                if(to <= 7) { // if the pawn is going onto the final row
                    // add a promotion for each possible piece
                    moveList.push_back(makeMove(from, to, PROMOTION, WN)); // promote to kight
                    moveList.push_back(makeMove(from, to, PROMOTION, WB)); // promote to bishop
                    moveList.push_back(makeMove(from, to, PROMOTION, WR)); // promote to bishop
                    moveList.push_back(makeMove(from, to, PROMOTION, WQ)); // promote to queen
                } else {
                    // normal pawn push
                    moveList.push_back(makeMove(from, to, QUIET));

                    // double push
                    // only if in the second row (SQ_A2-SQ_H2)
                    if (from >= SQ_A2 && from <= SQ_H2){
                        int doublePushTo = from + NORTH + NORTH;

                        if(!getBit(occupiedSquares, doublePushTo)){ // if not occupied
                            // do double push
                            moveList.push_back(makeMove(from, doublePushTo, DOUBLE_PUSH));
                        }
                    }
                }

            } 
            
            // captures

            // capture left
            to = from + NORTH_WEST;
            if (to >= SQ_A8 && (from % 8) != 0){ // if still on board and not on file A (left most side of the baord)
                if(getBit(enemyPieces, to)){ // enemy piece available for capture
                    int capturedPiece = board.boardArr[to];
                    if(to <= 7) { // if the pawn is going onto the final row
                        // add a promotion for each possible piece with capture flag and pice
                        moveList.push_back(makeMove(from, to, PROMOTION | CAPTURE, WN, capturedPiece)); // promote to kight
                        moveList.push_back(makeMove(from, to, PROMOTION | CAPTURE, WB, capturedPiece)); // promote to bishop
                        moveList.push_back(makeMove(from, to, PROMOTION | CAPTURE, WR, capturedPiece)); // promote to bishop
                        moveList.push_back(makeMove(from, to, PROMOTION | CAPTURE, WQ, capturedPiece)); // promote to queen
                    } else {
                        // normal pawn push
                        moveList.push_back(makeMove(from, to, CAPTURE, 0, capturedPiece));
                    }
                } else if (to == board.ep_target) {
                    // capture via en pessant
                    moveList.push_back(makeMove(from, to, CAPTURE | EN_PASSANT, 0, BP));
                }

            }

            // capture right
            to = from + NORTH_EAST;
            if (to >= SQ_A8 && (from % 8) != 7){ // if still on board and not on file H (left most side of the baord)
                if(getBit(enemyPieces, to)){ // enemy piece available for capture
                    int capturedPiece = board.boardArr[to];
                    if(to <= SQ_H8) { // if the pawn is going onto the final row
                        // add a promotion for each possible piece with capture flag and pice
                        moveList.push_back(makeMove(from, to, PROMOTION | CAPTURE, WN, capturedPiece)); // promote to kight
                        moveList.push_back(makeMove(from, to, PROMOTION | CAPTURE, WB, capturedPiece)); // promote to bishop
                        moveList.push_back(makeMove(from, to, PROMOTION | CAPTURE, WR, capturedPiece)); // promote to bishop
                        moveList.push_back(makeMove(from, to, PROMOTION | CAPTURE, WQ, capturedPiece)); // promote to queen
                    } else {
                        // normal pawn push
                        moveList.push_back(makeMove(from, to, CAPTURE, 0, capturedPiece));
                    }
                } else if (to == board.ep_target) {
                    // capture via en pessant
                    moveList.push_back(makeMove(from, to, CAPTURE | EN_PASSANT, 0, BP));
                }

            }
        }

    } else {


        // while there are pawns on the board
        while(pawns) {
            int from = popLSB(pawns);
            int to;

            // single pawn push

            to = from + SOUTH; // move one square north

            if (to <= SQ_H1 && !getBit(occupiedSquares, to)){ // if still on the board and no occupied square in front of pawn

                if(to >= SQ_A1) { // if the pawn is going onto the final row
                    // add a promotion for each possible piece
                    moveList.push_back(makeMove(from, to, PROMOTION, BN)); // promote to kight
                    moveList.push_back(makeMove(from, to, PROMOTION, BB)); // promote to bishop
                    moveList.push_back(makeMove(from, to, PROMOTION, BR)); // promote to bishop
                    moveList.push_back(makeMove(from, to, PROMOTION, BQ)); // promote to queen
                } else {
                    // normal pawn push
                    moveList.push_back(makeMove(from, to, QUIET));

                    // double push
                    // only if in the second row (SQ_A2-SQ_H2)
                    if (from >= SQ_A7 && from <= SQ_H7){
                        int doublePushTo = from + SOUTH + SOUTH;

                        if(!getBit(occupiedSquares, doublePushTo)){ // if not occupied
                            // do double push
                            moveList.push_back(makeMove(from, doublePushTo, DOUBLE_PUSH));
                        }
                    }
                }

            } 
            
            // captures

            // capture left
            to = from + SOUTH_WEST;
            if (to <= SQ_H1 && (from % 8) != 0){ // if still on board and not on file A (left most side of the baord)
                if(getBit(enemyPieces, to)){ // enemy piece available for capture
                    int capturedPiece = board.boardArr[to];
                    if(to >= SQ_A1) { // if the pawn is going onto the final row
                        // add a promotion for each possible piece with capture flag and pice
                        moveList.push_back(makeMove(from, to, PROMOTION | CAPTURE, BN, capturedPiece)); // promote to kight
                        moveList.push_back(makeMove(from, to, PROMOTION | CAPTURE, BB, capturedPiece)); // promote to bishop
                        moveList.push_back(makeMove(from, to, PROMOTION | CAPTURE, BR, capturedPiece)); // promote to bishop
                        moveList.push_back(makeMove(from, to, PROMOTION | CAPTURE, BQ, capturedPiece)); // promote to queen
                    } else {
                        // normal pawn push
                        moveList.push_back(makeMove(from, to, CAPTURE, 0, capturedPiece));
                    }
                } else if (to == board.ep_target) {
                    // capture via en pessant
                    moveList.push_back(makeMove(from, to, CAPTURE | EN_PASSANT, 0, WP));
                }

            }

            // capture right
            to = from + SOUTH_EAST;
            if (to <= SQ_H1 && (from % 8) != 7){ // if still on board and not on file H (left most side of the baord)
                if(getBit(enemyPieces, to)){ // enemy piece available for capture
                    int capturedPiece = board.boardArr[to];
                    if(to >= SQ_A1) { // if the pawn is going onto the final row
                        // add a promotion for each possible piece with capture flag and pice
                        moveList.push_back(makeMove(from, to, PROMOTION | CAPTURE, BN, capturedPiece)); // promote to kight
                        moveList.push_back(makeMove(from, to, PROMOTION | CAPTURE, BB, capturedPiece)); // promote to bishop
                        moveList.push_back(makeMove(from, to, PROMOTION | CAPTURE, BR, capturedPiece)); // promote to bishop
                        moveList.push_back(makeMove(from, to, PROMOTION | CAPTURE, BQ, capturedPiece)); // promote to queen
                    } else {
                        // normal pawn push
                        moveList.push_back(makeMove(from, to, CAPTURE, 0, capturedPiece));
                    }
                } else if (to == board.ep_target) {
                    // capture via en pessant
                    moveList.push_back(makeMove(from, to, CAPTURE | EN_PASSANT, 0, WP));
                }

            }
        }
    }

}


bool MoveGen::isSquareAttacked(const Board& board, int square, int attackingColour){
    // is square attacked by a pawn?
    if(attackingColour == WHITE){
        // a white pawn attacks an enemy piece from the south west and south east
        // if the piece is not on the last row and not in file on edge and there is a white pawn in the attcking place then the square is attacked by white pawn
        if(square+SOUTH_WEST <= SQ_H1 && (square % 8 != 0) && board.boardArr[square+SOUTH_WEST] == WP) return true; 
        if(square+SOUTH_EAST <= SQ_H1 && (square % 8 != 7) && board.boardArr[square+SOUTH_EAST] == WP) return true;
    } else {
        // similar logic for black pawn

        if(square+NORTH_WEST >= SQ_A8 && (square % 8 != 0) && board.boardArr[square+NORTH_WEST] == BP) return true; 
        if(square+NORTH_EAST >= SQ_A8 && (square % 8 != 7) && board.boardArr[square+NORTH_EAST] == BP) return true;
    }

    // is square attacked by knight?
    // get all enemy knights
    U64 knights = (attackingColour == WHITE) ? board.bitboards[WN] : board.bitboards[BN];

    // look up if the attacked square is in the knight attack LUT
    // this is done by first finding all the places where a knight can attack the square (by pretending there is a knight there)
    // and seeing if there is a knight in any of those attack possitions
    if(knightAttacks[square] & knights) return true;

    // is the square attacked by king piece
    // similar logic to knights

    // get king position
    U64 king = (attackingColour == WHITE) ? board.bitboards[WK] : board.bitboards[BK];

    // look up if the attacked square is in the king attack LUT
    if(kingAttacks[square] & king) return true;

    // is square attacked by sliding piece
    // we can combine queens with the rook and bishop respectively as the queen moves in both ways
    U64 rooksQueens = (attackingColour == WHITE) ? (board.bitboards[WR] | board.bitboards[WQ]) : (board.bitboards[BR] | board.bitboards[BQ]);
    U64 bishopsQueens = (attackingColour == WHITE) ? (board.bitboards[WB] | board.bitboards[WQ]) : (board.bitboards[BB] | board.bitboards[BQ]);

    // same logic in generate sliding moves to check if we "hit" a piece
    // check orthogonal (rook/queen)
    int orthogonalDirs[] = {NORTH, SOUTH, EAST, WEST};
    for (int dir : orthogonalDirs) {
        int t = square + dir;
        while (t >= SQ_A8 && t <= SQ_H1) {
            // Edge wrap check
            // break when reach the edge
            if ((dir == EAST && t % 8 == 0) || (dir == WEST && t % 8 == 7)) break;
            
            int piece = board.boardArr[t];
            if (piece != -1) {
                if ((1ULL << t) & rooksQueens) return true; // Hit enemy rook/queen
                break; // Hit something else (blocker)
            }
            t += dir;
        }
    }

    // Check Diagonal (Bishop/Queen)
    int diagDirs[] = {NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST};
    for (int dir : diagDirs) {
        int t = square + dir;
        while (t >= SQ_A8 && t <= SQ_H1) {
            // break when reach the edge
            if ((dir == NORTH_EAST || dir == SOUTH_EAST) && t % 8 == 0) break;
            if ((dir == NORTH_WEST || dir == SOUTH_WEST) && t % 8 == 7) break;

            int piece = board.boardArr[t];
            if (piece != -1) {
                if ((1ULL << t) & bishopsQueens) return true;
                break;
            }
            t += dir;
        }
    }

    // passes all the checks for all pieces so must not be attacked
    return false;

}