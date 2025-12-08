import { useContext } from "react";

import type { File, Rank } from "../types/types";

import { GameContext } from "../context/GameContext";
import { squareToCoord } from "../utils/coordinateConverter";
import { ThemeContext } from "../context/ThemeContext";

interface SquareProps {
    rank: Rank;
    file: File;
}

function Square({ file, rank }: SquareProps) {
    const { bgWhite, bgBlack } = useContext(ThemeContext);
    const { playerColor, gameProgress, gameState } = useContext(GameContext);

    const coordinate = squareToCoord({file, rank});
    const piece = gameState.boardState[coordinate[0]][coordinate[1]];

    const playersTurn = gameProgress === "in progress" && (gameState.toMove === playerColor);

    return (
        <div className={`aspect-square ${(coordinate[0]%2 == coordinate[1]%2) ? bgWhite : bgBlack}`}>
            {
                piece.active && 
                <div className={`h-full w-full flex items-center justify-center select-none ${playersTurn ? "cursor-pointer" : "cursor-not-allowed"}`}>
                    <img src={`./assets/chess-pieces/${piece.type}-${piece.color}.svg`} className="w-4/5 aspect-square" />
                </div>
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
                <Square key={`${file}${rank}`} file={file} rank={rank} />
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
