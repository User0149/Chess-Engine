#include <emscripten/bind.h>

#include <chrono>
#include <random>
#include <algorithm>

#include "game_helper_funcs.h"
#include "utils.h"

using namespace emscripten;

// non-castling and non-pawn moves
std::vector<PossibleMove> normal_piece_moves(const GameState &game_state, Piece piece, Square square) {
    Coordinate piece_coords = square_to_coord(square);

    std::vector<PossibleMove> allowed_moves;

    // this piece attacks "normally"
    // keep moving in one direction until we are blocked
    for (Coordinate direction:piece.attack_directions()) {
        for (int r = 1; r <= piece.attack_range(); r++) {
            int dest_i = piece_coords.i + (r * direction.i);
            int dest_j = piece_coords.j + (r * direction.j);

            if (!valid_coord({dest_i, dest_j})) {
                break;
            }
            
            Piece dest_piece = game_state.board_state[dest_i][dest_j];
            if (dest_piece.active && dest_piece.color == game_state.to_move) { // can't move past a friendly piece
                break;
            }

            // we can move to this square (ignoring king checks)
            GameState new_game_state = game_state;
            bool dangerous = false;

            new_game_state.moves = game_state.moves + 1;
            if (dest_piece.active && dest_piece.color != game_state.to_move) {
                new_game_state.last_capture_or_pawn_move = game_state.moves + 1;

                // capturing with a greater valued piece is dangerous
                dangerous = (piece.value() >= dest_piece.value());
            }

            // move piece
            new_game_state.board_state[piece_coords.i][piece_coords.j].active = false;
            new_game_state.board_state[dest_i][dest_j] = piece;
            new_game_state.board_state[dest_i][dest_j].moves = piece.moves + 1;
            new_game_state.board_state[dest_i][dest_j].last_move_index = game_state.moves + 1;

            new_game_state.previous_states[new_game_state.hash()]++;

            if (piece.type == "king" && piece.moves == 0) {
                (game_state.to_move == "white" ? new_game_state.castling_advantage_white : new_game_state.castling_advantage_black) = -1.0;
            }

            if (!is_targeted(new_game_state, king_square(new_game_state))) { // we don't put our king in check, so this move is valid
                new_game_state.to_move = game_state.to_move == "white" ? "black" : "white";

                Move move = {square, coord_to_square({dest_i, dest_j}), piece.type, dangerous};
                allowed_moves.push_back({move, new_game_state});
            }

            if (dest_piece.active && dest_piece.color != game_state.to_move) { // opponent is on this square, so we can't move any further
                break;
            }
        }
    }

    return allowed_moves;
}

std::vector<PossibleMove> castling_moves(const GameState &game_state, Piece king, Square king_pos_square) {
    Coordinate king_coord = square_to_coord(king_pos_square);

    std::vector<PossibleMove> allowed_moves;

    for (char rook_file:{'a', 'h'}) { // test for queen- and kingside castling
        Coordinate rook_square_coord = square_to_coord({std::string(1, rook_file), king_pos_square.rank});
        Piece rook_square_piece = game_state.board_state[rook_square_coord.i][rook_square_coord.j];

        Square king_dest_square = {(rook_file == 'a' ? "c": "g"), king_pos_square.rank};
        Coordinate king_dest_coord = square_to_coord(king_dest_square);
        
        if (rook_square_piece.active && rook_square_piece.last_move_index == 0) { // rook hasn't moved: try castling

            bool can_castle = !is_targeted(game_state, king_pos_square);
            for (char file = std::min(rook_file, 'e') + 1; file <= std::max(rook_file, 'e') - 1; file++) { // check for pieces blocking the castling path
                Square in_between_square = {std::string(1, file), king_pos_square.rank};
                Coordinate in_between_coord = square_to_coord(in_between_square);
                if (game_state.board_state[in_between_coord.i][in_between_coord.j].active) {
                    can_castle = false;
                }
            }
            for (char file = std::min('e', king_dest_square.file[0]); file <= std::max('e', king_dest_square.file[0]); file++) { // check if king will be checked
                Square in_between_square = {std::string(1, file), king_pos_square.rank};
                if (is_targeted(game_state, in_between_square)) {
                    can_castle = false;
                }
            }

            if (can_castle) { // we can castle
                GameState new_game_state = game_state;

                new_game_state.moves = game_state.moves + 1;
                (game_state.to_move == "white" ? new_game_state.castling_advantage_white : new_game_state.castling_advantage_black) = 1.0;
                new_game_state.last_capture_or_pawn_move = game_state.last_capture_or_pawn_move; // castling is not a capture nor a pawn move

                // move king
                new_game_state.board_state[king_coord.i][king_coord.j].active = false;
                new_game_state.board_state[king_dest_coord.i][king_dest_coord.j] = king;
                new_game_state.board_state[king_dest_coord.i][king_dest_coord.j].last_move_index = game_state.moves + 1;
                new_game_state.board_state[king_dest_coord.i][king_dest_coord.j].moves = king.moves + 1;

                // move rook
                Coordinate rook_dest_coord = square_to_coord({std::string(1, rook_file == 'a' ? 'd': 'f'), king_pos_square.rank});
                new_game_state.board_state[rook_square_coord.i][rook_square_coord.j].active = false;
                new_game_state.board_state[rook_dest_coord.i][rook_dest_coord.j] = rook_square_piece;
                new_game_state.board_state[rook_dest_coord.i][rook_dest_coord.j].last_move_index = game_state.moves + 1;
                
                new_game_state.previous_states[new_game_state.hash()]++;

                if (!is_targeted(new_game_state, king_square(new_game_state))) { // i don't see how our king can be targeted if it doesn't step through targeted squares but let's include this check anyway
                    new_game_state.to_move = game_state.to_move == "white" ? "black" : "white";

                    Move move = {king_pos_square, king_dest_square, "king", false};
                    allowed_moves.push_back({move, new_game_state});
                }
            }
        }
    }

    return allowed_moves;
}

std::vector<PossibleMove> pawn_non_en_passant_moves(const GameState &game_state, Piece pawn, Square pawn_square) {
    Coordinate pawn_coord = square_to_coord(pawn_square);
    int move_direction_rank = (pawn.color == "white" ? 1 : -1);

    std::vector<PossibleMove> allowed_moves;

    std::vector<Coordinate> potential_dest_coords;

    // add forward moves
    Square forward_1_square = {pawn_square.file, std::string(1, pawn_square.rank[0] + move_direction_rank)};
    Coordinate forward_1_coord = square_to_coord(forward_1_square);
    if (!game_state.board_state[forward_1_coord.i][forward_1_coord.j].active) { // can move forward one square
        potential_dest_coords.push_back(forward_1_coord);

        Square forward_2_square = {pawn_square.file, std::string(1, pawn_square.rank[0] + 2*move_direction_rank)};
        Coordinate forward_2_coord = square_to_coord(forward_2_square);
        if (pawn.last_move_index == 0 && !game_state.board_state[forward_2_coord.i][forward_2_coord.j].active) { // first move and can move 2 squares forward
            potential_dest_coords.push_back(forward_2_coord);
        }
    }
    
    // add capture moves
    std::vector<Coordinate> capture_coords = {
        square_to_coord({
            std::string(1, pawn_square.file[0] - 1),
            std::string(1, pawn_square.rank[0] + move_direction_rank)
        }), 
        square_to_coord({
            std::string(1, pawn_square.file[0] + 1),
            std::string(1, pawn_square.rank[0] + move_direction_rank)
        })
    };

    for (Coordinate capture_coord:capture_coords) {
        if (valid_coord(capture_coord) && game_state.board_state[capture_coord.i][capture_coord.j].color != game_state.to_move && game_state.board_state[capture_coord.i][capture_coord.j].active) {
            potential_dest_coords.push_back(capture_coord);
        }
    }

    // test if the found moves are possible
    for (Coordinate dest_coord:potential_dest_coords) {
        Square dest_square = coord_to_square(dest_coord);

        std::vector<std::string> new_piece_types = ((dest_square.rank == "1" || dest_square.rank == "8") ? (std::vector<std::string>){"queen", "rook", "bishop", "knight"} : (std::vector<std::string>){"pawn"});
        
        for (std::string new_piece_type:new_piece_types) {
            Piece dest_piece = game_state.board_state[dest_coord.i][dest_coord.j];
            GameState new_game_state = game_state;

            // promotions and captures of pawns are dangerous
            bool dangerous = (dest_piece.active && dest_piece.color != game_state.to_move && pawn.value() >= dest_piece.value()) || new_piece_type != "pawn";

            new_game_state.moves = game_state.moves + 1;
            new_game_state.last_capture_or_pawn_move = game_state.moves + 1;

            // move piece
            new_game_state.board_state[pawn_coord.i][pawn_coord.j].active = false;
            new_game_state.board_state[dest_coord.i][dest_coord.j] = pawn;
            new_game_state.board_state[dest_coord.i][dest_coord.j].type = new_piece_type;
            new_game_state.board_state[dest_coord.i][dest_coord.j].last_move_index = game_state.moves + 1;
            new_game_state.board_state[dest_coord.i][dest_coord.j].moves = pawn.moves + 1;

            new_game_state.previous_states[new_game_state.hash()]++;

            if (!is_targeted(new_game_state, king_square(new_game_state))) { // we don't put our king in check, so this move is valid
                new_game_state.to_move = game_state.to_move == "white" ? "black" : "white";

                Move move = {pawn_square, dest_square, new_piece_type, dangerous};
                allowed_moves.push_back({move, new_game_state});
            }
        }
    }

    return allowed_moves;
}

std::vector<PossibleMove> pawn_en_passant_moves(const GameState &game_state, Piece pawn, Square pawn_square) {
    Coordinate pawn_coord = square_to_coord(pawn_square);
    int move_direction_rank = (pawn.color == "white" ? 1 : -1);

    std::vector<PossibleMove> allowed_moves;

    if (pawn_square.rank == (game_state.to_move == "white" ? "5" : "4")) {
        std::vector<int> potential_file_increments;
        for (int file_inc:{-1, 1}) {
            Coordinate adjacent_coord = square_to_coord({
                std::string(1, pawn_square.file[0] + file_inc),
                pawn_square.rank
            });
            Coordinate dest_coord = square_to_coord({
                std::string(1, pawn_square.file[0] + file_inc),
                std::string(1, pawn_square.rank[0] + move_direction_rank)
            });

            if (valid_coord(adjacent_coord)) {
                Piece adjacent_piece = game_state.board_state[adjacent_coord.i][adjacent_coord.j];

                if (adjacent_piece.active && adjacent_piece.color != game_state.to_move && adjacent_piece.type == "pawn" && adjacent_piece.last_move_index == game_state.moves && adjacent_piece.moves == 1) { // we can capture this pawn en passant
                    GameState new_game_state = game_state;

                    new_game_state.moves = game_state.moves + 1;
                    new_game_state.last_capture_or_pawn_move = game_state.moves + 1;

                    // move pawn
                    new_game_state.board_state[pawn_coord.i][pawn_coord.j].active = false;
                    new_game_state.board_state[dest_coord.i][dest_coord.j] = pawn;
                    new_game_state.board_state[dest_coord.i][dest_coord.j].moves = pawn.moves + 1;
                    new_game_state.board_state[dest_coord.i][dest_coord.j].last_move_index = game_state.moves + 1;

                    // delete opponent pawn
                    new_game_state.board_state[adjacent_coord.i][adjacent_coord.j].active = false;

                    new_game_state.previous_states[new_game_state.hash()]++;

                    if (!is_targeted(new_game_state, king_square(new_game_state))) { // we don't put our king in check, so this move is valid
                        new_game_state.to_move = game_state.to_move == "white" ? "black" : "white";
                        Move move = {pawn_square, coord_to_square(dest_coord), "pawn", false};
                        allowed_moves.push_back({move, new_game_state});
                    }
                }
            }
        }
    }

    return allowed_moves;
}

std::vector<PossibleMove> pawn_moves(const GameState &game_state, Piece pawn, Square pawn_square) {
    std::vector<PossibleMove> allowed_moves;

    for (PossibleMove pawn_move:pawn_non_en_passant_moves(game_state, pawn, pawn_square)) {
        allowed_moves.push_back(pawn_move);
    }

    for (PossibleMove pawn_move:pawn_en_passant_moves(game_state, pawn, pawn_square)) {
        allowed_moves.push_back(pawn_move);
    }

    return allowed_moves;
}

std::vector<PossibleMove> possible_moves(const GameState &game_state) {
    std::vector<PossibleMove> allowed_moves;

    for (int i = 0; i <= 7; i++) {
        for (int j = 0; j <= 7; j++) {
            Piece piece = game_state.board_state[i][j];
            Square square = coord_to_square({i, j});
            if (piece.active && piece.color == game_state.to_move){ // brute force through all our pieces
                if (piece.type != "pawn") {
                    for (PossibleMove move:normal_piece_moves(game_state, piece, square)) {
                        allowed_moves.push_back(move);
                    }              
                }
                if (piece.type == "king" && piece.last_move_index == 0) { // king hasn't moved: check for castling
                    for (PossibleMove castling_move:castling_moves(game_state, piece, square)) {
                        allowed_moves.push_back(castling_move);
                    }                    
                }
                if (piece.type == "pawn") {
                    for (PossibleMove pawn_move:pawn_moves(game_state, piece, square)) {
                        allowed_moves.push_back(pawn_move);
                    }
                }
            }
        }
    }

    // randomise moves
    static std::mt19937 rng(time(0));
    std::shuffle(allowed_moves.begin(), allowed_moves.end(), rng);

    // put the best moves first and randomise equal moves by stable sorting
    std::stable_sort(allowed_moves.begin(), allowed_moves.end(), [](const PossibleMove a, const PossibleMove b) {
        return a.game_state.eval() < b.game_state.eval();
    });

    return allowed_moves;
}

EMSCRIPTEN_BINDINGS(possible_moves_lib) {
    register_vector<PossibleMove>("PossibleMoveVector");
    function("possibleMoves", &possible_moves);
}
