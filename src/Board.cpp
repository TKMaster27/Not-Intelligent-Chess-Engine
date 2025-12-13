#include "Board.hpp"
#include <iostream>
#include <sstream>


// default constructor uses start position as starting config
Board::Board(): Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"){}

// will fill respective bitboards
Board::Board(const std::string &fenConfig) {

  // convert string to stream for processing
  std::istringstream ss(fenConfig);

  std::string position, castlingStr, epStr;

  // extract relevant information from the fen string
  ss >> position >> activeColour >> castlingStr >> epStr >> halfMoves >> fullMoves;


  initBitBoards(position);

  // Parse Castling Rights to Integer
    castlingRights = 0;
    if (castlingStr != "-") {
        for (char c : castlingStr) {
            switch(c) {
                case 'K': castlingRights |= WK_CA; break;
                case 'Q': castlingRights |= WQ_CA; break;
                case 'k': castlingRights |= BK_CA; break;
                case 'q': castlingRights |= BQ_CA; break;
            }
        }
    }

    // 3. Parse En Passant Target to Integer
    if (epStr != "-") {
        ep_target = convertCordToSquare(epStr);
    } else {
        ep_target = NO_SQ;
    }

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

        boardArr[current++] = NO_PIECE;
      }
    } else if (std::isalpha(c)) {
        
        // when a piece is in the fen string, update the corresponding bitboard        
        switch(c) {
          case 'P': 
            bitboards[WP] |= 1ULL << current;
            boardArr[current] = WP;
            break;
          case 'N': 
            bitboards[WN] |= 1ULL << current; 
            boardArr[current] = WN;
            break;
          case 'B': 
            bitboards[WB] |= 1ULL << current; 
            boardArr[current] = WB;
            break;
          case 'R': 
            bitboards[WR] |= 1ULL << current; 
            boardArr[current] = WR;
            break;
          case 'Q': 
            bitboards[WQ] |= 1ULL << current; 
            boardArr[current] = WQ;
            break;
          case 'K': 
            bitboards[WK] |= 1ULL << current; 
            boardArr[current] = WK;
            break;
          case 'p': 
            bitboards[BP] |= 1ULL << current; 
            boardArr[current] = BP;
            break;
          case 'n': 
            bitboards[BN] |= 1ULL << current; 
            boardArr[current] = BN;
            break;
          case 'b': 
            bitboards[BB] |= 1ULL << current; 
            boardArr[current] = BB;
            break;
          case 'r': 
            bitboards[BR] |= 1ULL << current; 
            boardArr[current] = BR;
            break;
          case 'q': 
            bitboards[BQ] |= 1ULL << current; 
            boardArr[current] = BQ;
            break;
          case 'k': 
            bitboards[BK] |= 1ULL << current; 
            boardArr[current] = BK;
            break;
        }

        current++;

    } else if (c == '/') { // ignore slashes
      continue;
    }

  }


  // geneerate occupance bitboards
  bitboards[BLACK_OCC] = bitboards[BP] | bitboards[BN] | bitboards[BB] | bitboards[BR] | bitboards[BQ] | bitboards[BK];
  bitboards[WHITE_OCC] = bitboards[WP] | bitboards[WN] | bitboards[WB] | bitboards[WR] | bitboards[WQ] | bitboards[WK];

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
  boardArr[from] = NO_PIECE;

  if(flags & DOUBLE_PUSH) {
    ep_target = (activeColour == WHITE) ? to + 8 : to - 8;
  } else {
    ep_target = NO_SQ;
  }
  

  // handle captures
  if(flags & CAPTURE) {
    // update halfmoves
    halfMoves = 0;

    if (flags & EN_PASSANT) {
      int capSq = (activeColour == WHITE) ? to + 8: to - 8;
      bitboards[capturedPiece] &= ~(1ULL << capSq);
      boardArr[capSq] = NO_PIECE;
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
        case SQ_G1:          // White Kingside (g1)
          rookFrom = SQ_H1; rookTo = SQ_F1; 
          rookPiece = WR;
          break;
        case SQ_C1:  // White Queenside (c1)
          rookFrom = SQ_A1; rookTo = SQ_D1; 
          rookPiece = WR;
          break;
        case SQ_G8:   // Black Kingside (g8)
          rookFrom = SQ_H8;  rookTo = SQ_F8;  
          rookPiece = BR;
          break;
        case SQ_C8:   // Black Queenside (c8)
          rookFrom = SQ_A8;  rookTo = SQ_D8;  
          rookPiece = BR;
          break;
      }

      // Move the Rook on the Bitboard
      bitboards[rookPiece] &= ~(1ULL << rookFrom); // Remove from corner
      bitboards[rookPiece] |= (1ULL << rookTo);   // Place next to King

      // Update the Mailbox
      boardArr[rookFrom] = NO_PIECE;        // Empty the corner
      boardArr[rookTo] = rookPiece;   // Place Rook
  }

  // Update Rights based on the PIECE MOVING (from)
  // If King moves, lose both rights for that side
  if (piece == WK) {
      castlingRights &= ~(WK_CA | WQ_CA);
  } else if (piece == BK) {
      castlingRights &= ~(BK_CA | BQ_CA);
  }
  // If Rook moves, lose right for that corner
  // (Using straightforward if-checks for clarity)
  if (from == SQ_H1) castlingRights &= ~WK_CA;
  else if (from == SQ_A1) castlingRights &= ~WQ_CA;
  else if (from == SQ_H8) castlingRights &= ~BK_CA;
  else if (from == SQ_A8) castlingRights &= ~BQ_CA;


  // Update Rights based on the PIECE CAPTURED (to)
  // If a rook is captured, the opponent loses castling rights on that side.
  // (e.g., if White takes a1, Black loses Queen-side castle).
  if (flags & CAPTURE) {
      if (to == SQ_H1) castlingRights &= ~WK_CA;
      else if (to == SQ_A1) castlingRights &= ~WQ_CA;
      else if (to == SQ_H8) castlingRights &= ~BK_CA;
      else if (to == SQ_A8) castlingRights &= ~BQ_CA;
  }

  // handle promotions
  if (flags & PROMOTION) {
    bitboards[promotionPiece] |= (1ULL << to);
    boardArr[to] = promotionPiece; // update mailbox with promoted piece
  } else {
    bitboards[piece] |= (1ULL << to);
    boardArr[to] = piece;

  }

  if (piece == WP || piece == BP || (flags & CAPTURE)) {
      halfMoves = 0; // Reset on pawn move or capture
  } else {
      halfMoves++;
  }

  if (activeColour == BLACK) {
      fullMoves++; // Increment full moves after Black's turn
  }

  // update active colour
  activeColour = (activeColour == WHITE) ? BLACK : WHITE;

  // update occupancy bitboards
  bitboards[BLACK_OCC] = bitboards[BP] | bitboards[BN] | bitboards[BB] | bitboards[BR] | bitboards[BQ] | bitboards[BK];
  bitboards[WHITE_OCC] = bitboards[WP] | bitboards[WN] | bitboards[WB] | bitboards[WR] | bitboards[WQ] | bitboards[WK];

  bitboards[ALL_OCC] = bitboards[BLACK_OCC] | bitboards[WHITE_OCC];
  
}

// Converts an index (0-63) to algebraic notation (e.g., 60 -> "e8")
std::string Board::convertSquareToCord(int square) {
    if (square < 0 || square > 63) return ""; // Safety check

    int rankIndex = square / 8;
    int fileIndex = square % 8;

    // Convert file index to char ('a' + 0 = 'a', 'a' + 1 = 'b'...)
    char file = 'a' + fileIndex;
    
    // Convert rank index to char (Row 0 is Rank '8', Row 7 is Rank '1')
    char rank = '8' - rankIndex;

    return {file, rank};
}

// Converts algebraic notation to an index (e.g., "e8" -> 60)
int Board::convertCordToSquare(const std::string &cord) {
    if (cord.length() != 2) return -1; // Invalid format

    int file = cord[0] - 'a';       // 'a' - 'a' = 0
    int rank = '8' - cord[1];       // '8' - '8' = 0 (Top row), '8' - '1' = 7 (Bottom row)

    // Check bounds
    if (file < 0 || file > 7 || rank < 0 || rank > 7) {
        return -1;
    }

    return rank * 8 + file;
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
        case WP: result+="P "; break;
        case WN: result+="N "; break;
        case WB: result+="B "; break;
        case WR: result+="R "; break;
        case WQ: result+="Q "; break;
        case WK: result+="K "; break;
        case BP: result+="p "; break;
        case BN: result+="n "; break;
        case BB: result+="b "; break;
        case BR: result+="r "; break;
        case BQ: result+="q "; break;
        case BK: result+="k "; break;
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
