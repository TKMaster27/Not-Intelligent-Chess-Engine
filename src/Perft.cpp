
#include <iostream>
#include <vector>
#include "Perft.hpp"
#include "MoveGen.hpp"
#include "Move.hpp"
#include "BitUtils.hpp"

uint64_t Perft::perft(Board& board, int depth){
    // base case
    if (depth == 0){
        return 1;
    }

    std::vector<Move> moves = MoveGen::generateMoves(board);

    uint64_t nodes = 0; // number is probably huge so ensure 64bit unsigned int

    int side = board.activeColour;

    int kingType = (side == WHITE) ? WK : BK;

    for(const Move& move: moves){
        Board nextBoard = board; // copy root board with copy assignment constructor 
        nextBoard.makeMove(move); // make the move on this "branch board"

        int kingSquare = -1;
        if(board.bitboards[kingType]) {
            kingSquare = getLSB(nextBoard.bitboards[kingType]);
        }

        // if king square is being attacked by enemy on next move, skip move
        if(MoveGen::isSquareAttacked(nextBoard, kingSquare, nextBoard.activeColour)){
            continue;
        }

        nodes += perft(nextBoard, depth-1);

    }

    return nodes;

}

void Perft::perftDivide(Board& board, int depth){
    std::cout << "\n--- Perft Divide (Depth " << depth << ") ---\n";

    std::vector<Move> moves = MoveGen::generateMoves(board);

    uint64_t totalNodes = 0; // number is probably huge so ensure 64bit unsigned int

    int side = board.activeColour;
    int kingType = (side == WHITE) ? WK : BK;

    for(const Move& move: moves){
        Board nextBoard = board; // copy root board with copy assignment constructor 
        nextBoard.makeMove(move); // make the move on this "branch board"

        int kingSquare = -1;
        if(board.bitboards[kingType]) {
            kingSquare = getLSB(nextBoard.bitboards[kingType]);
        }

        // if king square is being attacked by enemy on next move, skip move
        if(MoveGen::isSquareAttacked(nextBoard, kingSquare, nextBoard.activeColour)){
            continue;
        }

        // Calculate nodes just for this branch
        uint64_t branchNodes = perft(nextBoard, depth - 1);
        totalNodes += branchNodes;

        // print move and count
        std::cout << board.convertSquareToCord(fromSq(move)) 
                  << board.convertSquareToCord(toSq(move)) 
                  << ": " << branchNodes << "\n";
    }

    std::cout << "\nTotal Nodes: " << totalNodes << "\n";
    std::cout << "-------------------------------\n";
}

