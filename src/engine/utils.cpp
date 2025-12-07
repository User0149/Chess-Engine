#include <utils.h>

Coordinate square_to_coord(Square square) {
    Coordinate coord;
    coord.i = square.file - 'a';
    coord.j = square.rank - '1';
    return coord;
}
Square coord_to_square(Coordinate coord) {
    Square square = {'a' + coord.i, coord.j + '1'};
    return square;
}

bool valid_coord(Coordinate coord) {
    return 0 <= coord.i && coord.i <= 7 && 0 <= coord.j && coord.j <= 7;
}
