#ifndef CHESS_MOVEGEN_HPP
#define CHESS_MOVEGEN_HPP

#include "Board.hpp"
#include "Move.hpp"
#include <vector>

class MoveGen {
public:
    // The main function: Returns a list of moves for the current side
    static std::vector<Move> generateLegalMoves(const Board &board);
    static std::vector<Move> generateMoves(const Board& board);
    static bool isSquareAttacked(const Board& board, int square, int attackingColour);

private:
    // functions generate moves for specific pieces
    static void generatePawnMoves(const Board& board, std::vector<Move>& moveList);
    static void generateKnightMoves(const Board& board, std::vector<Move>& moveList);
    static void generateKingMoves(const Board& board, std::vector<Move>& moveList);
    static void generateSlidingMoves(const Board& board, std::vector<Move>& moveList); // Rooks, Bishops, Queens
};

#endif