#include <numeric>

#include "structs.h"

Coordinate square_to_coord(Square square) {
    Coordinate coord;
    coord.i = square.file[0] - 'a';
    coord.j = square.rank[0] - '1';
    return coord;
}

Square coord_to_square(Coordinate coord) {
    Square square = {std::string(1, 'a' + coord.i), std::string(1, coord.j + '1')};
    return square;
}

bool valid_coord(Coordinate coord) {
    return 0 <= coord.i && coord.i <= 7 && 0 <= coord.j && coord.j <= 7;
}

Coordinate simplified_direction_vector(Coordinate source, Coordinate dest) {
    if (source == dest) return {0, 0};

    Coordinate difference_vector = dest - source;
    int GCD = abs(std::gcd(difference_vector.i, difference_vector.j));

    difference_vector.i /= GCD;
    difference_vector.j /= GCD;

    return difference_vector;
}
