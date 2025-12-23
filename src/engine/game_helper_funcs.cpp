#include <emscripten/bind.h>

#include <algorithm>

#include "game_helper_funcs.h"
#include "utils.h"
#include "possible_moves.h"

using namespace emscripten;

Square king_square(const GameState &game_state) {
    for (int i = 0; i <= 7; i++) {
        for (int j = 0; j <= 7; j++) {
            Piece piece = game_state.board_state[i][j];
            if (piece.type == "king" && piece.color == game_state.to_move && piece.active) {
                return coord_to_square({i, j});
            }
        }
    }
    return {"-", "-"};
}

bool is_targeted(const GameState &game_state, Square test_square) {
    for (int i = 0; i <= 7; i++) {
        for (int j = 0; j <= 7; j++) {
            Piece piece = game_state.board_state[i][j];
            Coordinate coord = {i, j};
            Square square = coord_to_square(coord);

            if (piece.active && piece.color != game_state.to_move){ // brute force through all opponent pieces
                if (piece.type != "pawn") {
                    Coordinate needed_direction = simplified_direction_vector(coord, square_to_coord(test_square));
                    std::vector<Coordinate> attack_directions = piece.attack_directions();

                    if (find(attack_directions.begin(), attack_directions.end(), needed_direction) == attack_directions.end()) {
                        // move on to next piece; this piece cannot attack the test square
                        continue;
                    }

                    // check to see if the piece can reach test_square
                    for (int r = 1; r <= piece.attack_range(); r++) {
                        int dest_i = coord.i + (r * needed_direction.i);
                        int dest_j = coord.j + (r * needed_direction.j);

                        if (!valid_coord({dest_i, dest_j})) {
                            break;
                        }
                        
                        Piece dest_piece = game_state.board_state[dest_i][dest_j];
                        if (dest_piece.active && dest_piece.color != game_state.to_move) { // can't move past a friendly piece
                            break;
                        }

                        // opponent piece targets this square
                        Square dest_square = coord_to_square({dest_i, dest_j});
                        if (test_square == dest_square) {
                            return true;
                        }

                        if (dest_piece.active &&  dest_piece.color == game_state.to_move) { // we are on this square, so opponent can't move any further
                            break;
                        }
                    }
                }
                else {
                    int move_direction_rank = (piece.color == "white" ? 1 : -1);
                    std::vector<Square> capture_squares = {{std::string(1, square.file[0] - 1), std::string(1, square.rank[0] + move_direction_rank)}, {std::string(1, square.file[0] + 1), std::string(1, square.rank[0]  + move_direction_rank)}};

                    for (Square attacked_square:capture_squares) {
                        if (attacked_square == test_square) {
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

bool no_moves_left(const GameState &game_state) {
    for (int i = 0; i <= 7; i++) {
        for (int j = 0; j <= 7; j++) {
            Piece piece = game_state.board_state[i][j];
            Square square = coord_to_square({i, j});

            if (piece.active && piece.color == game_state.to_move){ // brute force through all our pieces
                if (piece.type != "pawn") {
                    if (!normal_piece_moves(game_state, piece, square).empty()) {
                        return false;
                    }              
                }
                if (piece.type == "king" && piece.last_move_index == 0) { // king hasn't moved: check for castling
                    if (!castling_moves(game_state, piece, square).empty()) {
                        return false;
                    }                    
                }
                if (piece.type == "pawn") {
                    if (!pawn_moves(game_state, piece, square).empty()) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

bool is_stalemate(const GameState &game_state) {
    return !is_targeted(game_state, king_square(game_state)) && no_moves_left(game_state);
}

bool is_checkmate(const GameState &game_state) {
    return is_targeted(game_state, king_square(game_state)) && no_moves_left(game_state);
}

bool threefold_repetition(const GameState &game_state) {
    for (auto [state, freq]:game_state.previous_states) {
        if (freq >= 3) {
            return true;
        }
    }
    return false;
}

bool fifty_move_rule(const GameState &game_state) {
    return game_state.moves - game_state.last_capture_or_pawn_move >= 100;
}

bool insufficient_material(const GameState &game_state) {
    std::vector<std::string> white_material;
    std::vector<std::string> black_material;
    for (int i = 0; i <= 7; i++) {
        for (int j = 0; j <= 7; j++) {
            Piece piece = game_state.board_state[i][j];
            if (piece.active) {
                std::string color = piece.color;

                std::string square_color = (i % 2 == j % 2 ? "light" : "dark");
                std::string piece_type = (piece.type == "bishop" ? (square_color == "light" ? "light bishop" : "dark bishop") : piece.type);

                if (color == "white") {
                    white_material.push_back(piece_type);
                }
                else {
                    black_material.push_back(piece_type);
                }
            }
        }
    }

    white_material.erase(std::find(white_material.begin(), white_material.end(), "king"));
    black_material.erase(std::find(black_material.begin(), black_material.end(), "king"));

    std::vector<std::string> minMaterial = (white_material.size() < black_material.size() ? white_material : black_material);
    std::vector<std::string> maxMaterial = (white_material.size() < black_material.size() ? black_material : white_material);

    if (minMaterial.size() == 0) { // lone king
        if (maxMaterial.size() == 0) { // two lone kings
            return true;
        }
        else if (maxMaterial.size() == 1) { // lone king vs king + bishop/knight
            return maxMaterial[0] == "dark bishop" || maxMaterial[0] == "light bishop" || maxMaterial[0] == "knight";
        }
        else { // lone king vs >=2 non-king pieces
            return false;
        }
    }
    else if (minMaterial.size() == 1 && maxMaterial.size() == 1){ // check for king + bishop vs king + bishop with same-coloured bishops
        return (minMaterial[0] == "dark bishop" && maxMaterial[0] == "dark bishop") || (minMaterial[0] == "light bishop" && maxMaterial[0] == "light bishop");
    }
    else {
        return false;
    }
}

bool is_draw(const GameState &game_state) {
    return (
        is_stalemate(game_state) || 
        threefold_repetition(game_state) || 
        fifty_move_rule(game_state) || 
        insufficient_material(game_state)
    );
}

EMSCRIPTEN_BINDINGS(game_helper_funcs) {
    function("isCheckmate", &is_checkmate);
    function("isStalemate", &is_stalemate);
    function("threefoldRepetition", &threefold_repetition);
    function("fiftyMoveRule", &fifty_move_rule);
    function("insufficientMaterial", &insufficient_material);
    function("isDraw", &is_draw);
}
