import { createContext, useContext, useEffect, useState, type ReactNode } from "react";

import { type BoardState, type File, type GameProgress, type GameState, type Move, type Piece, type PieceType, type PlayerColor, type PossibleMove, type Rank, type Square, type StateSetter } from "../types/types";

import { squareToCoord } from "../utils/coordinateConverter";
import { toBoardState, toPieceVectorVector, toRecordStringNumber, toStringIntMap } from "../utils/jsEmbindConverter";

import { EngineContext } from "./EngineContext";

interface IGameContext {
    gameProgress: GameProgress;
    playerColor: PlayerColor;
    gameState: GameState;
    lastMove: Move;

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
    lastMove: {
        source: { file: "a", rank: "1" },
        dest: { file: "a", rank: "1" },
        newPiece: "pawn"
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
    
    const [lastMove, setLastMove] = useState<Move>({
        source: { file: "a", rank: "1" },
        dest: { file: "a", rank: "1" },
        newPiece: "pawn"
    });

    const engine: any = useContext(EngineContext);

    const initialBoardState = () => {
        const initialBoard: BoardState = Array.from({ length: 8 }, () => Array.from({ length: 8 }, () => ({
            active: false,
            color: "white",
            type: "pawn",

            moves: 0,
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

                        moves: 0,
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
                    
                    moves: 0,
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

    useEffect(() => {
        if (gameProgress == "in progress" && gameState.toMove !== playerColor) { // do computer's move
            console.log("Making computer move");
            const computerMove: PossibleMove = engine.randomMove({
                ...gameState,
                previousStates: toStringIntMap(engine, gameState.previousStates),
                boardState: toPieceVectorVector(engine, gameState.boardState)
            });
            makeMove(computerMove);
        }
    }, [gameState, gameProgress]);

    const makeMove = (move: PossibleMove) => {
        setGameState({
            ...move.gameState,
            previousStates: toRecordStringNumber(move.gameState.previousStates),
            boardState: toBoardState(move.gameState.boardState)
        });
        setLastMove(move.move);
    }

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
        lastMove,

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
