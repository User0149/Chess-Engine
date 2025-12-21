import type { GameState, PieceType, PlayerColor } from "../types/types";

import { toEngineGameState } from "../utils/jsEmbindConverter";

function isCheckmate(engine: any, gameState: GameState) {
    if (!engine.isCheckmate) return false;
    return engine.isCheckmate(toEngineGameState(engine, gameState));
}

function isStalemate(engine: any, gameState: GameState) {
    if (!engine.isStalemate) return false;
    return engine.isStalemate(toEngineGameState(engine, gameState));
}

function threefoldRepetition(gameState: GameState) {
    return Math.max(...Object.values(gameState.previousStates)) >= 3;
}

function fiftyMoveRule(gameState: GameState) {
    return gameState.moves - gameState.lastCaptureOrPawnMove >= 100;
}

function insufficientMaterial(gameState: GameState): boolean {
    const material: () => Record<"whiteMaterial" | "blackMaterial", (Exclude<PieceType, "bishop"> | "light bishop" | "dark bishop")[]> = () => {
        const materialCount: Record<"whiteMaterial" | "blackMaterial", (Exclude<PieceType, "bishop"> | "light bishop" | "dark bishop")[]> = {
            whiteMaterial: [],
            blackMaterial: []
        };

        for (let i = 0; i <= 7; i++) {
            for (let j = 0; j <= 7; j++) {
                const piece = gameState.boardState[i][j];
                const color = piece.color;

                const squareColor = (i%2 == j%2 ? "light" : "dark");
                const pieceType: Exclude<PieceType, "bishop"> | "light bishop" | "dark bishop" = (piece.type === "bishop" ? (squareColor == "light" ? "light bishop" : "dark bishop"): piece.type);

                if (color === "white") {
                    materialCount.whiteMaterial.push(pieceType);
                }
                else {
                    materialCount.blackMaterial.push(pieceType);
                }
            }
        }

        return materialCount;
    };

    const { whiteMaterial , blackMaterial} = material();
    const whiteMaterialWithoutKing = whiteMaterial.filter(piece => piece !== "king");
    const blackMaterialWithoutKing = blackMaterial.filter(piece => piece !== "king");

    const minMaterial = (whiteMaterialWithoutKing.length < blackMaterialWithoutKing.length ? whiteMaterialWithoutKing : blackMaterialWithoutKing);
    const maxMaterial = (whiteMaterialWithoutKing.length < blackMaterialWithoutKing.length ? blackMaterialWithoutKing : whiteMaterialWithoutKing);

    if (minMaterial.length === 0) { // lone king
        if (maxMaterial.length === 0) { // two lone kings
            return true;
        }
        else if (maxMaterial.length === 1) { // lone king vs king + bishop/knight
            return maxMaterial[0] === "dark bishop" || maxMaterial[0] === "light bishop" || maxMaterial[0] === "knight";
        }
        else { // lone king vs >=2 non-king pieces
            return false;
        }
    }
    else if (minMaterial.length === 1 && maxMaterial.length === 1){ // check for king + bishop vs king + bishop with same-coloured bishops
        return (minMaterial[0] === "dark bishop" && maxMaterial[0] === "dark bishop") || (minMaterial[0] === "light bishop" && maxMaterial[0] === "light bishop");
    }
    else {
        return false;
    }
}

export function gameResult(gameState: GameState, playerColor: PlayerColor, engine: any) {
    if (isCheckmate(engine, gameState)) {
        return `${gameState.toMove === playerColor ? "Computer" : "You"} won by checkmate.`;
    }
    if (isStalemate(engine, gameState)) {
        return "Draw by stalemate."
    }
    if (threefoldRepetition(gameState)) {
        return "Draw by repetition.";
    }
    if (fiftyMoveRule(gameState)) {
        return "Draw by 50-move rule.";
    }
    if (insufficientMaterial(gameState)) {
        return "Draw by insufficient material.";
    }
    return "Game has not ended. You should not be seeing this.";
}
