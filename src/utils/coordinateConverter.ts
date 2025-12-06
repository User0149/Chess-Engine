import type { Coordinate, Square } from "../types/types";

export function squareToIndices(square: Square): Coordinate {
    return [square.file.charCodeAt(0) - "a".charCodeAt(0), Number(square.rank) - 1];
}
