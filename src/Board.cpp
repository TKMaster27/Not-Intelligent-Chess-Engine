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

  // extract relevant information from the fen string
  ss >> position >> activeColour >> castling >> ep_target >> halfMoves >> fullMoves;


  initBitBoards(position);

}

Board::~Board(){}

void Board::initBitBoards(const std::string &pos){

  // initialize the bitboards to all zeros
  for (size_t i = 0; i < 16; i++) {
    bitboards[i] = 0;
  }

  int current = 0;

  // for each character in the fen string
  for (char c : pos) {
    // if the character is a number skip that many squares
    if (std::isdigit(c)) {
      int skips = c - '0';
      for(int i = 0; i < skips; ++i ){

        boardArr[current++] = -1;
      }
    } else if (std::isalpha(c)) {
        
        // when a piece is in the fen string, update the corresponding bitboard        
        switch(c) {
          case 'P': 
            bitboards[WHITE_PAWN] |= 1ULL << current;
            boardArr[current] = WHITE_PAWN;
            break;
          case 'N': 
            bitboards[WHITE_KNIGHT] |= 1ULL << current; 
            boardArr[current] = WHITE_KNIGHT;
            break;
          case 'B': 
            bitboards[WHITE_BISHOP] |= 1ULL << current; 
            boardArr[current] = WHITE_BISHOP;
            break;
          case 'R': 
            bitboards[WHITE_ROOK] |= 1ULL << current; 
            boardArr[current] = WHITE_ROOK;
            break;
          case 'Q': 
            bitboards[WHITE_QUEEN] |= 1ULL << current; 
            boardArr[current] = WHITE_QUEEN;
            break;
          case 'K': 
            bitboards[WHITE_KING] |= 1ULL << current; 
            boardArr[current] = WHITE_KING;
            break;
          case 'p': 
            bitboards[BLACK_PAWN] |= 1ULL << current; 
            boardArr[current] = BLACK_PAWN;
            break;
          case 'n': 
            bitboards[BLACK_KNIGHT] |= 1ULL << current; 
            boardArr[current] = BLACK_KNIGHT;
            break;
          case 'b': 
            bitboards[BLACK_BISHOP] |= 1ULL << current; 
            boardArr[current] = BLACK_BISHOP;
            break;
          case 'r': 
            bitboards[BLACK_ROOK] |= 1ULL << current; 
            boardArr[current] = BLACK_ROOK;
            break;
          case 'q': 
            bitboards[BLACK_QUEEN] |= 1ULL << current; 
            boardArr[current] = BLACK_QUEEN;
            break;
          case 'k': 
            bitboards[BLACK_KING] |= 1ULL << current; 
            boardArr[current] = BLACK_KING;
            break;
        }

        current++;

    } else if (c == '/') { // ignore slashes
      continue;
    }

  }


  // geneerate occupance bitboards
  bitboards[BLACK_OCC] = bitboards[BLACK_PAWN] | bitboards[BLACK_KNIGHT] | bitboards[BLACK_BISHOP] | bitboards[BLACK_ROOK] | bitboards[BLACK_QUEEN] | bitboards[BLACK_KING];
  bitboards[WHITE_OCC] = bitboards[WHITE_PAWN] | bitboards[WHITE_KNIGHT] | bitboards[WHITE_BISHOP] | bitboards[WHITE_ROOK] | bitboards[WHITE_QUEEN] | bitboards[WHITE_KING];

  bitboards[ALL_OCC] = bitboards[BLACK_OCC] | bitboards[WHITE_OCC];

}

void Board::makeMove(Move m){
  // extract move data
  int from = fromSq(m);
  int to = toSq(m);
  int flags = moveFlags(m);
  int piece = boardArr[from];
  int capturedPiece = captured(m);
  int promotionPiece = promo(m);

  // remove piece from src square
  bitboards[piece] &= ~(1ULL << from);
  
  // update mailbox
  boardArr[from] = -1;
  

  // handle captures
  if(flags & CAPTURE) {
    if (flags & EN_PASSANT) {
      int capSq = (activeColour == WHITE) ? to + 8: to - 8;
      bitboards[capturedPiece] &= ~(1ULL << capSq);
      boardArr[capSq] = -1;
    } else {
      bitboards[capturedPiece] &= ~(1ULL << to);
    }
  }

  // handle castling
  if (flags & CASTLING) {
      int rookFrom, rookTo;
      int rookPiece;

      // Determine squares based on King's destination
      switch(to) {
        case 62:          // Black Kingside (g8)
          rookFrom = 63; rookTo = 61; 
          rookPiece = WHITE_ROOK;
          break;
        case 58:  // Black Queenside (c8)
          rookFrom = 56; rookTo = 59; 
          rookPiece = WHITE_ROOK;
          break;
        case 6:   // White Kingside (g1)
          rookFrom = 7;  rookTo = 5;  
          rookPiece = BLACK_ROOK;
          break;
        case 2:   // White Queenside (c1)
          rookFrom = 0;  rookTo = 3;  
          rookPiece = BLACK_ROOK;
          break;
      }

      // Move the Rook on the Bitboard
      bitboards[rookPiece] &= ~(1ULL << rookFrom); // Remove from corner
      bitboards[rookPiece] |= (1ULL << rookTo);   // Place next to King

      // Update the Mailbox
      boardArr[rookFrom] = -1;        // Empty the corner
      boardArr[rookTo] = rookPiece;   // Place Rook
  }

  // handle promotions
  if (flags & PROMOTION) {
    bitboards[promotionPiece] |= (1ULL << to);
    boardArr[to] = promotionPiece; // update mailbox with promoted piece
  } else {
    bitboards[piece] |= (1ULL << to);
    boardArr[to] = piece;

  }

  // update active colour
  activeColour = (activeColour == WHITE) ? BLACK : WHITE;

  // update occupancy bitboards
  bitboards[BLACK_OCC] = bitboards[BLACK_PAWN] | bitboards[BLACK_KNIGHT] | bitboards[BLACK_BISHOP] | bitboards[BLACK_ROOK] | bitboards[BLACK_QUEEN] | bitboards[BLACK_KING];
  bitboards[WHITE_OCC] = bitboards[WHITE_PAWN] | bitboards[WHITE_KNIGHT] | bitboards[WHITE_BISHOP] | bitboards[WHITE_ROOK] | bitboards[WHITE_QUEEN] | bitboards[WHITE_KING];
  bitboards[ALL_OCC] = bitboards[BLACK_OCC] | bitboards[WHITE_OCC];
  
}

// print out the current board state to the console
void Board::printBoard() const {
  std::string result = "+---+---+---+---+---+---+---+---+\n";

  for (size_t i = 0; i < 8; i++) {
    for (size_t j = 0; j < 8; j++) {
      result+= "| ";

      // get current piece code
      int piece = boardArr[i*8+j];

      switch(piece) {
        case WHITE_PAWN: result+="P "; break;
        case WHITE_KNIGHT: result+="N "; break;
        case WHITE_BISHOP: result+="B "; break;
        case WHITE_ROOK: result+="R "; break;
        case WHITE_QUEEN: result+="Q "; break;
        case WHITE_KING: result+="K "; break;
        case BLACK_PAWN: result+="p "; break;
        case BLACK_KNIGHT: result+="n "; break;
        case BLACK_BISHOP: result+="b "; break;
        case BLACK_ROOK: result+="r "; break;
        case BLACK_QUEEN: result+="q "; break;
        case BLACK_KING: result+="k "; break;
        default: result+="  ";
      }

    }
    result+="|\n+---+---+---+---+---+---+---+---+\n";

  }


  std::cout << result;
}

// print the bitboard of the given index to the console
void Board::printBitBoard(const int index) const {
  std::string result = "+---+---+---+---+---+---+---+---+\n";

  for (size_t i = 0; i < 8; i++) {
    for (size_t j = 0; j < 8; j++) {
      int posIndex = i*8+j;
      result+= "| ";
      result+= (1ULL<<posIndex & bitboards[index]) > 0 ? "x" : " ";
      result+= " ";

    }
    result+="|\n+---+---+---+---+---+---+---+---+\n";

  }


  std::cout << result;

}
