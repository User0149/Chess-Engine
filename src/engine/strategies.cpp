#include <emscripten/bind.h>

#include <algorithm>

#include "strategies.h"
#include "possible_moves.h"
#include "game_helper_funcs.h"

using namespace emscripten;

const int DEPTH = 3;
const double MOBILITY_FACTOR = 0.02;
const double CASTLING_FACTOR = 0.75;
const double INF = 1e9;

// evaluates how much advantage the player to move has
double eval(const GameState& game_state, const int depth, const double alpha, const bool dangerous) {
    if (is_checkmate(game_state)) {
        return -INF;
    }
    if (is_draw(game_state)) {
        return 0.0;
    }

    if (depth <= 0 && !dangerous) { // base case: simply count material advantage on the board
        return game_state.eval();
    }
    std::vector<PossibleMove> next_moves = possible_moves(game_state);

    GameState opp_game_state = game_state;
    opp_game_state.to_move = (game_state.to_move == "white" ? "black" : "white");

    int player_moves = (int)next_moves.size();
    int opponent_moves = (int)possible_moves(opp_game_state).size();

    double additional_advantage = MOBILITY_FACTOR * (player_moves - opponent_moves) + CASTLING_FACTOR * (game_state.to_move == "white" ? 1.0 : -1.0) * (game_state.castling_advantage_white - game_state.castling_advantage_black);

    // search through our moves
    double base_advantage = -INF;
    for (PossibleMove move:next_moves) {
        double beta = std::max(std::min(base_advantage + additional_advantage, INF), -INF); // advantage that we can force
        if (beta > -alpha) { // this move is worse for the opponent than their best move so far
            return INF;
        }

        base_advantage = std::max(base_advantage, -eval(move.game_state, depth - 1, beta, move.move.dangerous));
    }

    return std::max(std::min(base_advantage + additional_advantage, INF), -INF);
}

PossibleMove negamax_move(const GameState &game_state, const int depth) {
    std::vector<PossibleMove> next_moves = possible_moves(game_state);

    GameState opp_game_state = game_state;
    opp_game_state.to_move = (game_state.to_move == "white" ? "black" : "white");

    int player_moves = (int)next_moves.size();
    int opponent_moves = (int)possible_moves(opp_game_state).size();

    double additional_advantage = MOBILITY_FACTOR * (player_moves - opponent_moves) + CASTLING_FACTOR * (game_state.to_move == "white" ? 1.0 : -1.0) * (game_state.castling_advantage_white - game_state.castling_advantage_black);

    // play the move that maximises our advantage
    double base_advantage = - 2 * INF; // must always be overridden
    PossibleMove best_move;
    for (PossibleMove move:next_moves) {
        double alpha = std::max(std::min(base_advantage + additional_advantage, INF), -INF);
        double eval_child = -eval(move.game_state, depth - 1, alpha, move.move.dangerous);

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
