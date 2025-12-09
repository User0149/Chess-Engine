#pragma once

#include <vector>
#include "structs.h"

struct PossibleMove;
struct GameState;

std::vector<PossibleMove> possible_moves(const GameState &game_state);
