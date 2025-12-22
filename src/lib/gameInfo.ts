import type { GameState, PlayerColor } from "../types/types";

import { toEngineGameState } from "../utils/jsEmbindConverter";

function isCheckmate(engine: any, gameState: GameState) {
    if (!engine.isCheckmate) return false;
    return engine.isCheckmate(toEngineGameState(engine, gameState));
}

function isStalemate(engine: any, gameState: GameState) {
    if (!engine.isStalemate) return false;
    return engine.isStalemate(toEngineGameState(engine, gameState));
}

function threefoldRepetition(engine: any, gameState: GameState) {
    if (!engine.threefoldRepetition) return false;
    return engine.threefoldRepetition(toEngineGameState(engine, gameState));
}

function fiftyMoveRule(engine: any, gameState: GameState) {
    if (!engine.fiftyMoveRule) return false;
    return engine.fiftyMoveRule(toEngineGameState(engine, gameState));
}

function insufficientMaterial(engine: any, gameState: GameState): boolean {
    if (!engine.insufficientMaterial) return false;
    return engine.insufficientMaterial(toEngineGameState(engine, gameState));
}

export function isGameOver(engine: any, gameState: GameState) {
    return isCheckmate(engine, gameState) || isStalemate(engine, gameState) || threefoldRepetition(engine, gameState) || fiftyMoveRule(engine, gameState) || insufficientMaterial(engine, gameState);
}

export function gameResult(engine: any, gameState: GameState, playerColor: PlayerColor) {
    if (isCheckmate(engine, gameState)) {
        return `${gameState.toMove === playerColor ? "Computer" : "You"} won by checkmate.`;
    }
    if (isStalemate(engine, gameState)) {
        return "Draw by stalemate."
    }
    if (threefoldRepetition(engine, gameState)) {
        return "Draw by repetition.";
    }
    if (fiftyMoveRule(engine, gameState)) {
        return "Draw by 50-move rule.";
    }
    if (insufficientMaterial(engine, gameState)) {
        return "Draw by insufficient material.";
    }
    return "Game has not ended. You should not be seeing this.";
}
