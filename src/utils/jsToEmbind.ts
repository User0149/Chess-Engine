import type { BoardState } from "../types/types";

export function toStringIntMap(engine: any, obj: Record<string, number>) {
    const map = new engine.StringIntMap();
    for (const k in obj) {
        map.set(k, obj[k]);
    }
    return map;
}

export function toPieceVectorVector(engine: any, boardState: BoardState) {
    const embindBoard = new engine.PieceVectorVector();

    for (const row of boardState) {
        const embindRow = new engine.PieceVector();
        for (const piece of row) {
            embindRow.push_back(piece);
        }

        embindBoard.push_back(embindRow);
    }

    return embindBoard;
}
