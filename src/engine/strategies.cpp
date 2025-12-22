#include <emscripten/bind.h>

#include <algorithm>

#include "strategies.h"
#include "possible_moves.h"
#include "game_helper_funcs.h"

using namespace emscripten;

// evaluates how much advantage the player to move has
double eval(const GameState& game_state, const int depth) {
    if (is_checkmate(game_state)) {
        return -INFINITY;
    }
    if (is_stalemate(game_state)) {
        return 0.0;
    }

    if (depth == 0) { // base case: simply count material advantage on the board
        return game_state.eval();
    }

    std::vector<PossibleMove> next_moves = possible_moves(game_state);

    double advantage = -INFINITY;
    for (PossibleMove move:next_moves) {
        advantage = std::max(advantage, -eval(move.game_state, depth - 1));
    }

    return advantage;
}

PossibleMove random_move(const GameState &game_state) {
    std::vector<PossibleMove> next_moves = possible_moves(game_state);
    return next_moves[0];
}

PossibleMove minimax_move(const GameState &game_state, const int depth) {
    std::vector<PossibleMove> next_moves = possible_moves(game_state);

    // play the move that minimises opponent's advantage
    return *std::min_element(next_moves.begin(), next_moves.end(), [depth](const PossibleMove a, const PossibleMove b){
        return eval(a.game_state, depth - 1) < eval(b.game_state, depth - 1);
    });
}

PossibleMove greedy_move(const GameState &game_state) {
    return minimax_move(game_state, 1);
}

PossibleMove computer_move(const GameState &game_state) {
    const int DEPTH = 2;
    return minimax_move(game_state, DEPTH);
}

EMSCRIPTEN_BINDINGS(strategies) {
    function("computerMove", &computer_move);
}
