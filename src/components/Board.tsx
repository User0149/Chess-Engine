import { useContext } from "react";

import type { Coordinate, File, PieceType, PossibleMove, Rank, Square } from "../types/types";

import { squareToCoord } from "../utils/coordinateConverter";
import { toEngineGameState, toJSPossibleMove } from "../utils/jsEmbindConverter";

import { GameContext } from "../context/GameContext";
import { ThemeContext } from "../context/ThemeContext";
import { EngineContext } from "../context/EngineContext";

interface BoardSquareProps {
    rank: Rank;
    file: File;
}

function BoardSquare({ file, rank }: BoardSquareProps) {
    const { bgWhite, bgBlack, bgSelected } = useContext(ThemeContext);
    const { playerColor, gameProgress, gameState, lastMove, makeMove } = useContext(GameContext);
    const engine: any = useContext(EngineContext);

    const coordinate = squareToCoord({ file, rank });
    const piece = gameState.boardState[coordinate[0]][coordinate[1]];

    const squareInvolvedInLastMove = gameState.moves >= 1 && ((lastMove.source.file === file && lastMove.source.rank === rank) || (lastMove.dest.file == file && lastMove.dest.rank === rank));
    const playersTurn = gameProgress === "in progress" && (gameState.toMove === playerColor);

    const dropHandler = (e: React.DragEvent) => {
        e.preventDefault();

        const source: Square = JSON.parse(e.dataTransfer.getData("source"));
        const sourceCoord: Coordinate = squareToCoord(source);
        const sourcePieceType: PieceType = gameState.boardState[sourceCoord[0]][sourceCoord[1]].type;

        const dest: Square = { file, rank };
        
        const possibleMovesArray = engine.possibleMoves(toEngineGameState(engine, gameState));
        const moveOptions: PossibleMove[] = [];

        for (let i = 0; i < possibleMovesArray.size(); i++) {
            const possibleMove = possibleMovesArray.get(i);
            if (possibleMove.move.source.file === source.file && possibleMove.move.source.rank === source.rank &&
                possibleMove.move.dest.file === dest.file && possibleMove.move.dest.rank === dest.rank) {
                // this is a legal move
                moveOptions.push(toJSPossibleMove(possibleMove));
            }
        }

        if (moveOptions.length === 0) { // illegal move
            return;
        }
        else if (moveOptions.length === 1) { // non-promotion move
            makeMove(moveOptions[0]);
        }
        else if (moveOptions.length === 4) { // promotion
            console.log("Promotion not supported yet.");
        }
        else {
            throw new Error("Error: what is this move");
        }
    };

    return (
        <div className={`aspect-square relative ${(coordinate[0]%2 == coordinate[1]%2) ? bgBlack : bgWhite}`}
            onDragOver={(e) => {
                e.preventDefault();
            }}
            onDrop={(e) => {
                dropHandler(e);
            }}
        >
            {
                piece.active && 
                <div className={`h-full w-full flex items-center justify-center select-none ${playersTurn ? "cursor-pointer" : "cursor-not-allowed"}`}>
                    <img src={`./assets/chess-pieces/${piece.type}-${piece.color}.svg`} className="w-4/5 aspect-square z-100"
                        onDragStart={(e) => {
                            e.dataTransfer.setData("source", JSON.stringify({ file, rank }));
                        }}
                    />
                </div>
            }
            {
                squareInvolvedInLastMove && <div className={`absolute inset-0 opacity-50 ${bgSelected} h-full w-full`}></div>
            }
        </div>
    );
}

export default function Board() {
    const { playerColor, gameProgress } = useContext(GameContext);

    const Squares = [];

    const ranks: Rank[] = ["8", "7", "6", "5", "4", "3", "2", "1"];
    const files: File[] = ["a", "b", "c", "d", "e", "f", "g", "h"];

    if (playerColor === "black") {
        ranks.reverse();
        files.reverse();
    }

    for (const rank of ranks) {
        for (const file of files) {
            Squares.push(
                <BoardSquare key={`${file}${rank}`} file={file} rank={rank} />
            );
        }
    }

    return (
        <div className="h-full w-full p-4">
            <div className="board h-full aspect-square grid grid-cols-8 relative">
                {Squares}

                {gameProgress !== "in progress" && <div className="absolute inset-0 h-full w-full bg-black opacity-20"></div>}
            </div>
        </div>
    );
}
