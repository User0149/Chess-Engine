#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>

struct Coordinate {
    int i = 0;
    int j = 0;
};

struct Square {
    char file = 'a';
    char rank = '1';
};

struct Move {
    Square source = {'a', '1'};
    Square dest = {'a', '1'};
    std::string promote_to = "";
};

struct Piece {
    bool active = false;

    std::string color = "white";
    std::string type = "pawn";

    int moves = 0;
    int last_move_index = 0;

    double value() {
        if (type == "pawn") return 1.0;
        if (type == "knight") return 3.0;
        if (type == "bishop") return 3.0;
        if (type == "rook") return 5.0;
        if (type == "queen") return 9.0;
        if (type == "king") return 100.0;
        return 0.0;
    }

    int attack_range() {
        if (type == "pawn") return 1;
        if (type == "knight") return 1;
        if (type == "bishop") return 8;
        if (type == "rook") return 8;
        if (type == "queen") return 8;
        if (type == "king") return 1;
        return 0.0;
    }

    std::vector<Coordinate> attack_directions() {
        if (type == "pawn") return {};
        if (type == "knight") return {{1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1}};
        if (type == "bishop") return {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
        if (type == "rook") return {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
        if (type == "queen") return {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
        if (type == "king") return {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
        return {};
    }
};

struct GameState {
    int moves = 0;
    std::map<std::string, int> previous_states;
    int last_capture_or_pawn_move = 0;
    std::string to_move = "white";

    std::vector<std::vector<Piece>> board_state = std::vector<std::vector<Piece>>(8, std::vector<Piece>(8));

    GameState(int _moves, std::map<std::string, int> _previous_states, int _last_capture_or_pawn_move, std::string _to_move, std::vector<std::vector<Piece>> _board_state) {
        moves = _moves;
        previous_states = _previous_states;
        to_move = _to_move;
        board_state = _board_state;
    }

    std::string hash() { // TODO: this hash doesn't take into account en passant and castling rights when hashing the state
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
};

struct PossibleMove {
    Move move;
    GameState game_state;
};
