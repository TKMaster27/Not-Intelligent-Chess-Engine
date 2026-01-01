#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <random>
#include <chrono>

#include "UCI.hpp"
#include "MoveGen.hpp"
#include "Move.hpp"
#include "Board.hpp"

// converts engine moves into uci strings
std::string moveToString(Move m, Board &board){
    // concatonate from square and to square
    std::string result = board.convertSquareToCord(fromSq(m)) + board.convertSquareToCord(toSq(m));

    //manage flags with promotion
    if(promo(m) & PROMOTION) {
        int promotedTo = promo(m);

        if(promotedTo == WN || promotedTo == BN) result += 'n';
        else if(promotedTo == WB || promotedTo == BB) result += 'b';
        else if(promotedTo == WR || promotedTo == BR) result += 'r';
        else result += 'q';
    }

    return result;
}

// find the move that corresponses to uci input
Move parseMove(std::string moveString, Board &board){
    std::vector<Move> moves = MoveGen::generateMoves(board);

    for(Move m: moves){
        if (moveToString(m, board) == moveString){
            return m;
        }
    }

    return 0;
}

void UCI::loop(){
    Board board;
    std::string line, token;

    // get random seed number from time
    unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();

    std::mt19937 g(seed);

    std::setbuf(stdin, NULL);
    std::setbuf(stdout, NULL);

    while(std::getline(std::cin, line)){
        std::stringstream ss(line);
        ss >> token;

        if(token == "uci"){
            std::cout << "id name NICE 1.0" << std::endl;
            std::cout << "id author Tarique Mackay" << std::endl;
            std::cout << "uciok" << std::endl;
        } else if (token == "isready") {
            std::cout << "readyok" << std::endl;
        } else if (token == "ucinewgame") {
            board = Board();
        } else if (token == "position") {
            ss >> token;

            if (token == "startpos") {
               board = Board();
               ss >> token;
            } else if (token == "fen") {
                ss >> token; // get fen string token
                board = Board(token);
                // ss >> token;
            }

            while (ss >> token){
                if (token == "moves") continue;
                Move m = parseMove(token, board);
                if (m != 0) { // move is valid in current position 
                    board.makeMove(m);
                }
            }

            //board.printBoard();
        } else if (token == "go") {
            
            // generate all possible moves
            std::vector<Move> moves = MoveGen::generateMoves(board);

            if(!moves.empty()){
                int randIndex = g() % moves.size();
                Move bestMove = moves[randIndex];
            

                std::cout << "bestmove " << moveToString(bestMove, board) << std::endl;
            } else {
               std::cout << "bestmove (none)" << std::endl; 
            }

        } else if (token == "print") {
            board.printBoard(); 
        } else if (token == "quit"){
            break;
        } else if (token == "stop"){
            continue;
        } else {
            std::cerr << "Invalid UCI command:\t" << token;
        }
    }
}
