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
    std::string file = "a";
    std::string rank = "1";
};

struct Move {
    Square source = {"a", "1"};
    Square dest = {"1", "1"};
    std::string new_piece = "pawn";
};

struct Piece {
    bool active = false;

    std::string color = "white";
    std::string type = "pawn";

    int moves = 0;
    int last_move_index = 0;

    double value();

    int attack_range();

    std::vector<Coordinate> attack_directions();
};

struct GameState {
    int moves = 0;
    std::map<std::string, int> previous_states;
    int last_capture_or_pawn_move = 0;
    std::string to_move = "white";

    std::vector<std::vector<Piece>> board_state = std::vector<std::vector<Piece>>(8, std::vector<Piece>(8));
    
    GameState() = default;

    GameState(int _moves, std::map<std::string, int> _previous_states, int _last_capture_or_pawn_move, std::string _to_move, std::vector<std::vector<Piece>> _board_state) {
        moves = _moves;
        previous_states = _previous_states;
        to_move = _to_move;
        board_state = _board_state;
    }

    std::string hash();
};

struct PossibleMove {
    Move move;
    GameState game_state;
};
