#include <emscripten/bind.h>

#include "structs.h"

using namespace emscripten;

double Piece::value() const {
    if (type == "pawn") return 1.0;
    if (type == "knight") return 3.0;
    if (type == "bishop") return 3.0;
    if (type == "rook") return 5.0;
    if (type == "queen") return 9.0;
    if (type == "king") return 100.0;
    return 0.0;
}

int Piece::attack_range() const {
    if (type == "pawn") return 1;
    if (type == "knight") return 1;
    if (type == "bishop") return 8;
    if (type == "rook") return 8;
    if (type == "queen") return 8;
    if (type == "king") return 1;
    return 0.0;
}

std::vector<Coordinate> Piece::attack_directions() const {
    if (type == "pawn") return {};
    if (type == "knight") return {{1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1}};
    if (type == "bishop") return {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    if (type == "rook") return {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    if (type == "queen") return {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    if (type == "king") return {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    return {};
}

std::string GameState::hash() const { // TODO: this hash doesn't take into account en passant and castling rights when hashing the state
    std::string position_string;
    for (int i = 0; i <= 7; i++) {
        for (int j = 0; j <= 7; j++) {
            Piece piece = board_state[i][j];
            position_string += (piece.active ? piece.color + piece.type : "-");
        }
    }

    std::string state = to_move + position_string;
    return std::to_string(std::hash<std::string>{}(state));
}

double GameState::eval() const {
    double advantage = 0.0;
    
    for (int i = 0; i <= 7; i++) {
        for (int j = 0; j <= 7; j++) {
            Piece piece = board_state[i][j];
            if (piece.active) {
                if (piece.color == to_move) {
                    advantage += piece.value();
                }
                else {
                    advantage -= piece.value();
                }
            }
        }
    }

    return advantage;
}

EMSCRIPTEN_BINDINGS(structs) {
    register_vector<std::string>("StringVector");
    register_vector<Piece>("PieceVector");
    register_vector<std::vector<Piece>>("PieceVectorVector");
    register_map<std::string, int>("StringIntMap");
    
    value_object<Coordinate>("Coordinate")
        .field("i", &Coordinate::i)
        .field("j", &Coordinate::j)
        ;
    value_object<Square>("Square")
        .field("file", &Square::file)
        .field("rank", &Square::rank)
        ;
    value_object<Move>("Move")
        .field("source", &Move::source)
        .field("dest", &Move::dest)
        .field("newPieceType", &Move::new_piece_type)
        ;
    value_object<Piece>("Piece")
        .field("active", &Piece::active)
        .field("color", &Piece::color)
        .field("type", &Piece::type)
        .field("moves", &Piece::moves)
        .field("lastMoveIndex", &Piece::last_move_index)
        ;
    value_object<GameState>("GameState")
        .field("moves", &GameState::moves)
        .field("previousStates", &GameState::previous_states)
        .field("lastCaptureOrPawnMove", &GameState::last_capture_or_pawn_move)
        .field("toMove", &GameState::to_move)
        .field("castlingAdvantageWhite", &GameState::castling_advantage_white)
        .field("castlingAdvantageBlack", &GameState::castling_advantage_black)
        .field("boardState", &GameState::board_state)
        ;
    value_object<PossibleMove>("PossibleMove")
        .field("move", &PossibleMove::move)
        .field("gameState", &PossibleMove::game_state)
        ;
}
