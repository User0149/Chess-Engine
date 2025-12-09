// #include <emscripten/bind.h>
#include "game_helper_funcs.h"
#include "possible_moves.h"
#include "utils.h"

// using namespace emscripten;

Square king_square(const GameState &game_state) {
    for (int i = 0; i <= 7; i++) {
        for (int j = 0; j <= 7; j++) {
            Piece piece = game_state.board_state[i][j];
            if (piece.type == "king" && piece.color == game_state.to_move && piece.active) {
                return coord_to_square({i, j});
            }
        }
    }
    return {'-', '-'};
}

bool is_targeted(const GameState &game_state, Square test_square) {
    for (int i = 0; i <= 7; i++) {
        for (int j = 0; j <= 7; j++) {
            Piece piece = game_state.board_state[i][j];
            Coordinate coord = {i, j};
            Square square = coord_to_square(coord);

            if (piece.active && piece.color != game_state.to_move){ // brute force through all opponent pieces
                if (piece.type != "pawn") {
                    for (Coordinate direction:piece.attack_directions()) {
                        for (int r = 1; r <= piece.attack_range(); r++) {
                            int dest_i = coord.i + (r * direction.i);
                            int dest_j = coord.j + (r * direction.j);

                            if (!valid_coord({dest_i, dest_j})) {
                                continue;
                            }
                            
                            Piece dest_piece = game_state.board_state[dest_i][dest_j];
                            if (!dest_piece.active || dest_piece.color == game_state.to_move) { // opponent piece targets this square
                                Square dest_square = coord_to_square({dest_i, dest_j});
                                if (test_square.file == dest_square.file && test_square.rank == dest_square.rank) {
                                    return true;
                                }

                                if (dest_piece.color == game_state.to_move) { // we are on this square, so opponent can't move any further
                                    break;
                                }
                            }
                        }
                    }
                }
                else {
                    int move_direction_rank = (piece.color == "white" ? 1 : -1);
                    std::vector<Square> capture_squares = {{char(square.file - 1), char(square.rank  + move_direction_rank)}, {char(square.file + 1), char(square.rank  + move_direction_rank)}};

                    for (Square attacked_square:capture_squares) {
                        if (attacked_square.file == test_square.file && attacked_square.rank && test_square.rank) {
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

bool is_stalemate(const GameState &game_state) {
    return possible_moves(game_state).empty() && !is_targeted(game_state, king_square(game_state));
}

bool is_checkmate(const GameState &game_state) {
    return possible_moves(game_state).empty() && is_targeted(game_state, king_square(game_state));
}


// EMSCRIPTEN_BINDINGS(game_helper_funcs) {
//     function("isStalemate", &is_stalemate);
//     function("isCheckmate", &is_checkmate);
// }
