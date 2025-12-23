#pragma once

#include "structs.h"

Coordinate square_to_coord(Square square);
Square coord_to_square(Coordinate coord);
bool valid_coord(Coordinate coord);
Coordinate simplified_direction_vector(Coordinate source, Coordinate dest);
