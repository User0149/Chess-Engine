#pragma once

#include "structs.h"

Square king_square(const GameState &game_state);
bool is_targeted(const GameState &game_state, Square square);

bool is_stalemate(const GameState &game_state);
bool is_checkmate(const GameState &game_state);
