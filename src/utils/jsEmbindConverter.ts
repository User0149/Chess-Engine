import type { BoardState, GameState, Piece, PossibleMove } from "../types/types";

export function toStringIntMap(engine: any, obj: Record<string, number>) {
    const map = new engine.StringIntMap();
    for (const key in obj) {
        map.set(key, obj[key]);
    }
    return map;
}

export function toRecordStringNumber(map: any): Record<string, number> {
    const mapKeys = map.keys();

    const obj: Record<string, number>  = {};
    for (let i = 0; i < mapKeys.size(); i++) {
        const key = mapKeys.get(i);
        obj[key] = map.get(key);
    }
    return obj;
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

export function toBoardState(pieceVectorVector: any): BoardState {
    const boardState: BoardState = [];

    for (let row = 0; row <= 7; row++) {
        const boardRow: Piece[] = [];
        for (let col = 0; col <= 7; col++) {
            boardRow.push(pieceVectorVector.get(row).get(col));
        }

        boardState.push(boardRow);
    }

    return boardState;
}

export function toEngineGameState(engine: any, gameState: GameState) {
    return ({
        ...gameState,
        previousStates: toStringIntMap(engine, gameState.previousStates),
        boardState: toPieceVectorVector(engine, gameState.boardState)
    });
}

export function toJSGameState(gameState: any): GameState {
    return ({
        ...gameState,
        previousStates: toRecordStringNumber(gameState.previousStates),
        boardState: toBoardState(gameState.boardState)
    });
}

export function toJSPossibleMove(possibleMove: any): PossibleMove {
    return ({
        move: possibleMove.move,
        gameState: toJSGameState(possibleMove.gameState)
    });
}
