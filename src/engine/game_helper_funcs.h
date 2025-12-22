#pragma once

#include "structs.h"

Square king_square(const GameState &game_state);
bool is_targeted(const GameState &game_state, Square square);

bool is_stalemate(const GameState &game_state);
bool is_checkmate(const GameState &game_state);
bool threefold_repetition(const GameState &game_state);
bool fifty_move_rule(const GameState &game_state);
bool insufficient_material(const GameState &game_state);

bool is_draw(const GameState &game_state);
