#include "Board.hpp"
#include <iostream>
#include <sstream>


// default constructor uses start position as starting config
Board::Board(): Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"){}

// will fill respective bitboards
Board::Board(const std::string &fenConfig) {

  // convert string to stream for processing
  std::istringstream ss(fenConfig);

  std:: string position;

  ss >> position >> activeColour >> castling >> ep_target >> halfMoves >> fullMoves;


  std::cout << position << std::endl;

  initBitBoards(position);

}

Board::~Board(){}

void Board::initBitBoards(const std::string &pos){

  // initialize the bitboards to all zeros
  for (size_t i = 0; i < 16; i++) {
    bitboards[i] = 0L;
  }

  int current = 0;
  for (char c : pos) {

    if (std::isdigit(c)) {
      int skips = c - '0';
      for(int i = 0; i < skips; ++i ){

        boardArr[current++] = ' ';
      }
    } else if (std::isalpha(c)) {

                
        switch(c) {
          case 'P': bitboards[WHITE_PAWN] |= 1UL << current; break;
          case 'N': bitboards[WHITE_KNIGHT] |= 1UL << current; break;
          case 'B': bitboards[WHITE_BISHOP] |= 1UL << current; break;
          case 'R': bitboards[WHITE_ROOK] |= 1UL << current; break;
          case 'Q': bitboards[WHITE_QUEEN] |= 1UL << current; break;
          case 'K': bitboards[WHITE_KING] |= 1UL << current; break;
          case 'p': bitboards[BLACK_PAWN] |= 1UL << current; break;
          case 'n': bitboards[BLACK_KNIGHT] |= 1UL << current; break;
          case 'b': bitboards[BLACK_BISHOP] |= 1UL << current; break;
          case 'r': bitboards[BLACK_ROOK] |= 1UL << current; break;
          case 'q': bitboards[BLACK_QUEEN] |= 1UL << current; break;
          case 'k': bitboards[BLACK_KING] |= 1UL << current; break;
        }

        boardArr[current++] = c;

    } else if (c == '/') {
      continue;
    }

  }


  bitboards[BLACK_OCC] |= bitboards[BLACK_PAWN] | bitboards[BLACK_KNIGHT] | bitboards[BLACK_BISHOP] | bitboards[BLACK_ROOK] | bitboards[BLACK_QUEEN] | bitboards[BLACK_KING];
  bitboards[WHITE_OCC] |= bitboards[WHITE_PAWN] | bitboards[WHITE_KNIGHT] | bitboards[WHITE_BISHOP] | bitboards[WHITE_ROOK] | bitboards[WHITE_QUEEN] | bitboards[WHITE_KING];

  bitboards[ALL_OCC] |= bitboards[BLACK_OCC] | bitboards[WHITE_OCC];

}

void Board::printBoard() const {
  std::string result = "+---+---+---+---+---+---+---+---+\n";

  for (size_t i = 0; i < 8; i++) {
    for (size_t j = 0; j < 8; j++) {
      result+= "| ";
      result+=boardArr[i*8+j];
      result+= " ";

    }
    result+="+\n+---+---+---+---+---+---+---+---+\n";
;
  }


  std::cout << result;
}
