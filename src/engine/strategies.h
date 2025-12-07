#pragma once
#include <random>
#include <chrono>
#include "structs.h"
#include "possible_moves.h"

PossibleMove random_move(const GameState &game_state);
