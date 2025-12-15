#include <iostream>
#include <string>
#include "Board.hpp"
#include "Move.hpp"
#include "Types.hpp"
#include "MoveGen.hpp"

int main(){

  // White Pawns: e2 (start), b5 (attacker), h7 (promoter)
    // Black Pieces: a6 (target), c6 (target)
    Board board("8/7P/P1p5/1P6/8/8/4P3/8 w - - 0 1");
    
    board.printBoard();
    std::cout << "\n--- Generating Pawn Moves ---" << std::endl;
    
    std::vector<Move> moves = MoveGen::generateMoves(board);

    int count = 0;
    for (Move m : moves) {
        std::cout << board.convertSquareToCord(fromSq(m)) 
                  << board.convertSquareToCord(toSq(m));
        
        // Print promotion suffix if needed
        if (promo(m)) {
            // Very basic char map for debug
            char pChar = ' ';
            switch(promo(m)) {
                case WQ: case BQ: pChar = 'q'; break;
                case WR: case BR: pChar = 'r'; break;
                case WB: case BB: pChar = 'b'; break;
                case WN: case BN: pChar = 'n'; break;
            }
            std::cout << pChar;
        }
        std::cout << " ";
        
        count++;
        if (count % 8 == 0) std::cout << "\n";
    }
    std::cout << "\nTotal Moves: " << moves.size() << std::endl;



  

  return 0;
}
