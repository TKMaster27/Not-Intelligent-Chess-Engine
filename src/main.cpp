#include <iostream>
#include <string>
#include "Board.hpp"
#include "Move.hpp"
#include "Types.hpp"

int main(){

  Board gameBoard;
  
  gameBoard.printBoard();

  std::cout << std::endl;

  // std::string move;

  // std::cin >> move;

  gameBoard.printBitBoard(14);



  

  return 0;
}
