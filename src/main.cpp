#include <iostream>
#include <string>
#include "Board.hpp"
#include "Move.hpp"
#include "Types.hpp"
#include "MoveGen.hpp"

int main(){

  // White Rook on D4, White Bishop on E4, Black Pawn on D7 (capture target)
  // FEN: 8/3p4/8/8/3RB3/8/8/8 w - - 0 1
  Board board("8/3p4/8/8/3RB3/8/8/8 w - - 0 1");
  
  board.printBoard();
  std::cout << "\n--- Generating Slider Moves ---" << std::endl;
  
  std::vector<Move> moves = MoveGen::generateMoves(board);

  int count = 0;
  for (Move m : moves) {
      std::cout << board.convertSquareToCord(fromSq(m)) 
                << board.convertSquareToCord(toSq(m)) << " ";
      count++;
      if (count % 8 == 0) std::cout << "\n"; // Newline every 8 moves
  }
  std::cout << "\nTotal Moves: " << moves.size() << std::endl;



  

  return 0;
}
