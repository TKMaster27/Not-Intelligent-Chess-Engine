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
#include "Search.hpp"

// converts engine moves into uci strings
std::string moveToString(Move m, Board &board){
    // concatonate from square and to square
    std::string result = board.convertSquareToCord(fromSq(m)) + board.convertSquareToCord(toSq(m));

    //manage flags with promotion
    if(moveFlags(m) & PROMOTION) {
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
    std::vector<Move> moves = MoveGen::generateLegalMoves(board);

    for(const Move& m: moves){
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

            // advdertising for lichess
            std::cout << "option name Move Overhead type spin default 10 min 0 max 5000" << std::endl;
            std::cout << "option name Threads type spin default 1 min 1 max 128" << std::endl;
            std::cout << "option name Hash type spin default 16 min 1 max 2048" << std::endl;

            std::cout << "uciok" << std::endl;
        } else if (token == "setoption") {
            continue; // TODO later perhaps
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
                std::string fen;
                while (ss >> token && token != "moves") {
                    fen += token + " ";
                }
                board = Board(fen); 
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

            int wtime = 0;
            int btime = 0;
            int winc = 0;
            int binc = 0;
            int movetime = 0;

            while (ss >> token){
                if (token == "wtime") ss >> wtime;
                else if (token == "btime") ss >> btime;
                else if (token == "winc") ss >> winc;
                else if (token == "binc") ss >> binc;
                else if (token == "movetime") ss >> movetime;
            }

            // thinking budget

            int timeToThink = 0;

            // if movetime given
            if(movetime != -1) {
                timeToThink = movetime;
            } else {
                // determine active colour and assign approprate time variables
                int time = (board.getSideToMove() == WHITE) ? wtime : btime;
                int inc = (board.getSideToMove() == WHITE) ? winc : binc; 

                // standard heuristic from google assuming game will be over in 20 moves
                timeToThink = (time / 20) + (inc / 2);
            }

            // failsafe minimum time
            if (timeToThink <= 0) timeToThink = 1000;
            
            // find best move
            Move bestMove = Search::searchPosition(board, 5000);

            if(bestMove != 0){   
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
