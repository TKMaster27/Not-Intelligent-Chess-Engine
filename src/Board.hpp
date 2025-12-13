#ifndef _Board_H
#define _Board_H 


#define WHITE_PAWN 0
#define WHITE_KNIGHT 1
#define WHITE_BISHOP 2
#define WHITE_ROOK   3
#define WHITE_QUEEN 4
#define WHITE_KING 5

#define BLACK_PAWN 6
#define BLACK_KNIGHT 7
#define BLACK_BISHOP 8
#define BLACK_ROOK 9
#define BLACK_QUEEN 10
#define BLACK_KING 11

#define WHITE_OCC 12
#define BLACK_OCC 13
#define ALL_OCC 14

#include <string>

class Board {
  private:
    unsigned long bitboards[16]; // represents the entrire board with an array of bitboards
    std::string activeColour;
    std::string castling;
    std::string ep_target;
    int halfMoves;
    int fullMoves;

    char boardArr[64]; 

    void initBitBoards(const std::string &pos);

  public:

    // defualt contructor
    Board();

    // paramaterised constructor given FEN string
    Board(const std::string &fenConfig);

    // destructor
    ~Board();

    

    // prints out a specific bitbaard given the index
    void printBitBoard(const int index) const;

    //print board
    void printBoard() const;
};

#endif // !Board

