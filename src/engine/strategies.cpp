#include <emscripten/bind.h>

#include <chrono>
#include <random>
#include <algorithm>

#include "strategies.h"
#include "possible_moves.h"

using namespace emscripten;

PossibleMove random_move(const GameState &game_state) {
    static std::mt19937 rng(time(0));
    std::vector<PossibleMove> moves = possible_moves(game_state);
    
    return moves[int(rng()%((int)moves.size()))];
}

// evaluates how much advantage the player to move has
double eval(const GameState& game_state, const int depth) {
    if (depth == 0) { // base case: simply count material advantage on the board
        return game_state.eval();
    }

    return 0.0;
}

PossibleMove minimax_move(const GameState &game_state, const int depth) {
    std::vector<PossibleMove> moves = possible_moves(game_state);

    // play the move that minimises opponent's advantage
    return *std::min_element(moves.begin(), moves.end(), [depth](const PossibleMove a, const PossibleMove b){
        return eval(a.game_state, depth - 1) < eval(b.game_state, depth - 1);
    });
}

PossibleMove greedy_move(const GameState &game_state) {
    return minimax_move(game_state, 1);
}

PossibleMove computer_move(const GameState &game_state) {
    return greedy_move(game_state);
}

EMSCRIPTEN_BINDINGS(strategies) {
    function("computerMove", &computer_move);
}
