#ifndef _Board_H
#define _Board_H 



#include <string>
#include <vector>
#include <cstdint>

#include "Types.hpp"
#include "Move.hpp"

class Board {
  private:
    U64 bitboards[16]; // represents the entrire board with an array of bitboards
    int activeColour;
    int castlingRights;
    int ep_target;
    int halfMoves;
    int fullMoves;

    int boardArr[64]; 

    void initBitBoards(const std::string &pos);

    std::string convertSquareToCord(int square);

    int convertCordToSquare(const std::string &cord);

  public:

    // defualt contructor
    Board();

    // paramaterised constructor given FEN string
    Board(const std::string &fenConfig);

    // destructor
    ~Board();
    
    // move a piece form one place to another place
    void makeMove(Move m);

    // generates all possible moves in the current possition
    

    // prints out a specific bitbaard given the index
    void printBitBoard(const int index) const;

    //print board
    void printBoard() const;
};

#endif // !Board

