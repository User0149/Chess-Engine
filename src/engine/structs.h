#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>

struct Coordinate {
    int i = 0;
    int j = 0;
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

    std::string hash() {
        std::string position_string;
        for (int i = 0; i <= 7; i++) {
            for (int j = 0; j <= 7; j++) {
                Piece piece = board_state[i][j];
                position_string += (piece.active ? piece.color + piece.type : "-");
            }
        }

        std::string moves_string;
        for (auto [move, new_game_state]:possible_moves(*this)) {
            moves_string += ((((("" + move.source.file) + move.source.rank) + move.dest.file) + move.dest.rank) + move.promote_to);
        }


        std::string state = to_move + position_string + moves_string;
        return std::to_string(std::hash<std::string>{}(state));
    }
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

struct PossibleMove {
    Move move;
    GameState game_state;
};
