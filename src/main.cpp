#include <iostream>
#include <string>
#include "Board.hpp"
#include "Move.hpp"
#include "Types.hpp"
#include "MoveGen.hpp"

int main(){

  // Setup a board with a White Knight on E4 and King on E1
  Board board("8/8/8/8/4N3/8/8/4K3 w - - 0 1");
  board.printBoard();

  std::cout << "\n--- Generating Moves ---" << std::endl;
  std::vector<Move> moves = MoveGen::generateMoves(board);

  for (Move m : moves) {
      std::cout << board.convertSquareToCord(fromSq(m)) 
                << board.convertSquareToCord(toSq(m)) << " ";
  }
  std::cout << "\nTotal Moves: " << moves.size() << std::endl;



  

  return 0;
}
