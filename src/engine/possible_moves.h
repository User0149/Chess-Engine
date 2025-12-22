#pragma once

#include <vector>
#include "structs.h"

std::vector<PossibleMove> normal_piece_moves(const GameState &game_state, Piece piece, Square square);
std::vector<PossibleMove> castling_moves(const GameState &game_state, Piece king, Square king_pos_square);
std::vector<PossibleMove> pawn_moves(const GameState &game_state, Piece pawn, Square pawn_square);

std::vector<PossibleMove> possible_moves(const GameState &game_state);
