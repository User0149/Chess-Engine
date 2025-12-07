import { createContext, useState, type ReactNode } from "react";

import type { BoardState, File, GameProgress, GameState, Piece, PieceType, PlayerColor, Rank, Square, StateSetter } from "../types/types";

import { squareToCoord } from "../utils/coordinateConverter";

interface IGameContext {
    gameProgress: GameProgress;
    playerColor: PlayerColor;
    gameState: GameState;

    setGameProgress: StateSetter<GameProgress>;
    setPlayerColor: StateSetter<PlayerColor>;
    setGameState: StateSetter<GameState>;

    resetGame: () => void;
    playAs: (color: PlayerColor) => void;
}

interface GameContextProviderProps {
    children: ReactNode;
}

export const GameContext = createContext<IGameContext>({
    gameProgress: "not started",
    playerColor: "white",
    gameState: {
        moves: 0,
        previousStates: {},
        lastCaptureOrPawnMove: 0,
        toMove: "white",
        boardState: [[]]
    },

    setGameProgress: () => {},
    setPlayerColor: () => {},
    setGameState: () => {},

    resetGame: () => {},
    playAs: () => {}
});

export default function GameContextProvider({ children }: GameContextProviderProps) {
    const [gameProgress, setGameProgress] = useState<GameProgress>("not started");
    const [playerColor, setPlayerColor] = useState<PlayerColor>("white");

    const initialBoardState = () => {
        const initialBoard: BoardState = Array.from({length: 8}, () => Array.from({length: 8}, () => ({
            active: false,
            color: "white",
            type: "pawn",
            lastMoveIndex: 0
        } as Piece)));

        const colors: PlayerColor[] = ["white", "black"];

        // put playing pieces into place
        type NonPawnPiece = Exclude<PieceType, "pawn">;
        const initialPieceFiles: Record<NonPawnPiece, File[]> = {
            rook: ["a", "h"],
            knight: ["b", "g"],
            bishop: ["c", "f"],
            queen: ["d"],
            king: ["e"]
        };
        const initialPieceRanks: Record<PlayerColor, Rank> = {
            white: "1",
            black: "8"
        };

        for (const playingPiece in initialPieceFiles) {
            for (const file of initialPieceFiles[playingPiece as NonPawnPiece]) {
                for (const color of colors) {
                    const rank = initialPieceRanks[color];

                    const square: Square = {file, rank};
                    const coord = squareToCoord(square);
                    initialBoard[coord[0]][coord[1]] = {
                        active: true,
                        color,
                        type: playingPiece as NonPawnPiece,
                        lastMoveIndex: 0
                    };
                }
            }
        }

        // put pawns into place
        const initialPawnRanks: Record<PlayerColor, Rank> = {
            white: "2",
            black: "7"
        };
        const initialPawnFiles: File[] = ["a", "b", "c", "d", "e", "f", "g", "h"];

        for (const file of initialPawnFiles) {
            for (const color of colors) {
                const rank = initialPawnRanks[color];

                const square: Square = {file, rank};
                const coord = squareToCoord(square);
                initialBoard[coord[0]][coord[1]] = {
                    active: true,
                    color,
                    type: "pawn",
                    lastMoveIndex: 0
                };
            }
        }

        return initialBoard;
    }

    const [gameState, setGameState] = useState<GameState>({
        moves: 0,
        previousStates: {},
        lastCaptureOrPawnMove: 0,
        toMove: "white",
        boardState: initialBoardState()
    });

    const resetGame = () => {
        setGameProgress("not started");
        setPlayerColor("white");
        setGameState({
            moves: 0,
            previousStates: {},
            lastCaptureOrPawnMove: 0,
            toMove: "white",
            boardState: initialBoardState()
        });
    };

    const playAs = (color: PlayerColor) => {
        setGameProgress("in progress");
        setPlayerColor(color);
    };

    const initialGameContext: IGameContext = {
        gameProgress,
        playerColor,
        gameState,

        setGameProgress,
        setPlayerColor,
        setGameState,

        resetGame,
        playAs
    };

    return (
        <GameContext value={initialGameContext}>
            {children}
        </GameContext>
    );
}