#ifndef CHESS_BOARD_HPP
#define CHESS_BOARD_HPP 



#include <string>
#include <vector>
#include <cstdint>

#include "Types.hpp"
#include "Move.hpp"



class Board {
  friend class MoveGen;
  friend class Perft;
  friend class Evaluation;
  friend class Search;
  private:
    U64 bitboards[16]; // represents the entrire board with an array of bitboards
    int activeColour;
    int castlingRights;
    int ep_target;
    int halfMoves;
    int fullMoves;

    int boardArr[64]; 

    void initBitBoards(const std::string &pos);

    

  public:

    // defualt contructor
    Board();

    // paramaterised constructor given FEN string
    Board(const std::string &fenConfig);

    // destructor
    ~Board();
    
    // move a piece form one place to another place
    void makeMove(Move m);

    std::string convertSquareToCord(int square) const;

    int convertCordToSquare(const std::string &cord) const;   

    // prints out a specific bitbaard given the index
    void printBitBoard(const int index) const;

    //print board
    void printBoard() const;

    int getSideToMove() const;
};

#endif // !Board

