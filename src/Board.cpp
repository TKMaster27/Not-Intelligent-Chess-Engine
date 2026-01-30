#include "Board.hpp"
#include "Move.hpp"
#include "BitUtils.hpp"
#include <iostream>
#include <sstream>


// default constructor uses start position as starting config
Board::Board(): Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"){}

// will fill respective bitboards
Board::Board(const std::string &fenConfig) {

  // convert string to stream for processing
  std::istringstream ss(fenConfig);

  std::string position, colourStr, castlingStr, epStr;

  // extract relevant information from the fen string
  ss >> position >> colourStr >> castlingStr >> epStr >> halfMoves >> fullMoves;

  activeColour = (colourStr == "w") ? WHITE : BLACK;

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

int Board::getSideToMove() const {
  return activeColour;
}

void Board::initBitBoards(const std::string &pos){

  // initialize the bitboards to all zeros
  for (size_t i = 0; i < 16; i++) {
    bitboards[i] = 0;
  }

  // Initialize boardArr to NO_PIECE
  for (int i = 0; i < 64; i++) {
      boardArr[i] = NO_PIECE;
  }

  int rank = 7; // Start at Top Rank
  int file = 0; // Start at File A

  // for each character in the fen string
  for (char c : pos) {
    if (c == '/') {
      rank--; // Move down one rank
      file = 0; // Reset file
      continue;
    }

    if (std::isdigit(c)) {
      int skips = c - '0';
      file += skips; // Skip empty squares
    } else {
      // Calculate Square Index: A1=0, H8=63
      int square = rank * 8 + file;
      
      int piece = NO_PIECE;
      switch(c) {
          case 'P': piece = WP; break;
          case 'N': piece = WN; break;
          case 'B': piece = WB; break;
          case 'R': piece = WR; break;
          case 'Q': piece = WQ; break;
          case 'K': piece = WK; break;
          case 'p': piece = BP; break;
          case 'n': piece = BN; break;
          case 'b': piece = BB; break;
          case 'r': piece = BR; break;
          case 'q': piece = BQ; break;
          case 'k': piece = BK; break;
      }

      if (piece != NO_PIECE) {
          setBit(bitboards[piece], square);
          boardArr[square] = piece;
      }
      
      file++; // Move to next file
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
    ep_target = (activeColour == WHITE) ? to - 8 : to + 8;
  } else {
    ep_target = NO_SQ;
  }
  

  // handle captures
  if(flags & CAPTURE) {
    // update halfmoves
    halfMoves = 0;

    if (flags & EN_PASSANT) {
      int capSq = (activeColour == WHITE) ? to - 8: to + 8;
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
std::string Board::convertSquareToCord(int square) const {
    if (square < 0 || square > 63) return ""; // Safety check

    int rankIndex = square / 8;
    int fileIndex = square % 8;

    // Convert file index to char ('a' + 0 = 'a', 'a' + 1 = 'b'...)
    char file = 'a' + fileIndex;
    
    // Convert rank index to char (Row 0 is Rank '8', Row 7 is Rank '1')
    char rank = '1' + rankIndex;

    return {file, rank};
}

// Converts algebraic notation to an index (e.g., "e8" -> 60)
int Board::convertCordToSquare(const std::string &cord) const {
    if (cord.length() != 2) return -1; // Invalid format

    int file = cord[0] - 'a';       // 'a' - 'a' = 0
    int rank = cord[1] - '1';       // '8' - '8' = 0 (Top row), '8' - '1' = 7 (Bottom row)

    // Check bounds
    if (file < 0 || file > 7 || rank < 0 || rank > 7) {
        return -1;
    }

    return rank * 8 + file;
}

// print out the current board state to the console
void Board::printBoard() const {
    std::string result;

    result += "    +---+---+---+---+---+---+---+---+\n";

    for (int rank = 7; rank >= 0; rank--) {
        // Rank number on the left
        result += " " + std::to_string(rank + 1) + "  ";

        for (int file = 0; file < 8; file++) {
            result += "| ";

            int piece = boardArr[rank * 8 + file];

            switch (piece) {
                case WP: result += "P "; break;
                case WN: result += "N "; break;
                case WB: result += "B "; break;
                case WR: result += "R "; break;
                case WQ: result += "Q "; break;
                case WK: result += "K "; break;
                case BP: result += "p "; break;
                case BN: result += "n "; break;
                case BB: result += "b "; break;
                case BR: result += "r "; break;
                case BQ: result += "q "; break;
                case BK: result += "k "; break;
                default: result += "  ";
            }
        }

        result += "|\n";
        result += "    +---+---+---+---+---+---+---+---+\n";
    }

    // File letters at the bottom
    result += "      a   b   c   d   e   f   g   h\n";

    std::cout << result;
}


// print the bitboard of the given index to the console
void Board::printBitBoard(const int index) const {
  std::string result = "+---+---+---+---+---+---+---+---+\n";

  for (int rank = 7; rank >= 0; rank--) {
    for (int file = 0; file < 8; file++) {
      int posIndex = rank*8+file;
      result+= "| ";
      result+= (1ULL<<posIndex & bitboards[index]) > 0 ? "x" : " ";
      result+= " ";

    }
    result+="|\n+---+---+---+---+---+---+---+---+\n";

  }


  std::cout << result;

}
