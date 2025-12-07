// #include <emscripten/bind.h>
#include "strategies.h"

// using namespace emscripten;

PossibleMove random_move(const GameState &game_state) {
    static std::mt19937 rng(time(0));
    std::vector<PossibleMove> moves = possible_moves(game_state);
    
    return moves[int(rng()%((int)moves.size()))];
}
