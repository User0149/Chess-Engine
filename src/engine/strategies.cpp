#include <emscripten/bind.h>

#include <algorithm>

#include "strategies.h"
#include "possible_moves.h"
#include "game_helper_funcs.h"

using namespace emscripten;

const int DEPTH = 3;
const double MOBILITY_FACTOR = 0.005;
const double CASTLING_FACTOR = 0.1;

// evaluates how much advantage the player to move has
double eval(const GameState& game_state, const int depth, double alpha = -INFINITY) {
    if (is_checkmate(game_state)) {
        return -INFINITY;
    }
    if (is_draw(game_state)) {
        return 0.0;
    }

    if (depth <= 0) { // base case: simply count material advantage on the board
        return game_state.eval();
    }
    
    std::vector<PossibleMove> next_moves = possible_moves(game_state);

    GameState opp_game_state = game_state;
    opp_game_state.to_move = (game_state.to_move == "white" ? "black" : "white");

    int player_moves = (int)next_moves.size();
    int opponent_moves = (int)possible_moves(opp_game_state).size();

    double additional_advantage = MOBILITY_FACTOR * (player_moves - opponent_moves) + CASTLING_FACTOR * (game_state.has_castled_white - game_state.has_castled_black);

    // search through our moves
    double base_advantage = -INFINITY;
    for (PossibleMove move:next_moves) {
        double beta = base_advantage + additional_advantage; // advantage that we can force
        if (beta > -alpha) { // this move is worse for the opponent than their best move so far
            return -alpha + 100;
        }

        base_advantage = std::max(base_advantage, -eval(move.game_state, depth - 1, beta));
    }


    return base_advantage + additional_advantage;
}

PossibleMove negamax_move(const GameState &game_state, const int depth) {
    std::vector<PossibleMove> next_moves = possible_moves(game_state);

    GameState opp_game_state = game_state;
    opp_game_state.to_move = (game_state.to_move == "white" ? "black" : "white");

    int player_moves = (int)next_moves.size();
    int opponent_moves = (int)possible_moves(opp_game_state).size();

    double additional_advantage = MOBILITY_FACTOR * (player_moves - opponent_moves) + CASTLING_FACTOR * (game_state.has_castled_white - game_state.has_castled_black);

    // play the move that maximises our advantage
    double base_advantage = -INFINITY;
    PossibleMove best_move;
    for (PossibleMove move:next_moves) {
        double alpha = base_advantage + additional_advantage;
        double eval_child = -eval(move.game_state, depth - 1, alpha);

        if (eval_child > base_advantage) {
            base_advantage = eval_child;
            best_move = move;
        }
    }

    return best_move;
}

PossibleMove computer_move(const GameState &game_state) {
    return negamax_move(game_state, DEPTH);
}

EMSCRIPTEN_BINDINGS(strategies) {
    function("computerMove", &computer_move);
}
