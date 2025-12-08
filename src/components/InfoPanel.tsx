import { useContext } from "react";

import { gameResult } from "../lib/gameInfo";

import { GameContext } from "../context/GameContext";
import { ThemeContext } from "../context/ThemeContext";

function PlayAsPrompt() {
    const { bgWhite, bgBlack } = useContext(ThemeContext);
    const { playAs } = useContext(GameContext);

    return (
        <div className="flex flex-col items-center text-center space-y-3">
            <div className="text-lg">Play as...</div>

            <div className={`p-1 w-2/3 border rounded-full cursor-pointer ${bgWhite}`} onClick={() => playAs("white")}>white</div>
            <div className={`p-1 w-2/3 border rounded-full cursor-pointer ${bgBlack}`} onClick={() => playAs("black")}>black</div>
            <div className="p-1 w-2/3 border rounded-full cursor-pointer bg-[gray]" onClick={() => playAs(Math.random() < 0.5 ? "white" : "black")}>random</div>
        </div>
    );
}

function ToMove() {
    const { gameState, playerColor } = useContext(GameContext);
    const colorToMove = gameState.toMove;

    return (
        <div className="text-center text-4xl">
            {(colorToMove === playerColor ? "Your" : "Computer's")} move
        </div>
    );
}

function GameResult() {
    const { gameState, playerColor } = useContext(GameContext);

    return (
        <div className="text-center text-4xl">
            {gameResult(gameState, playerColor)}
        </div>
    );
}

function NewGameButton() {
    const { bgWhite } = useContext(ThemeContext);
    const { resetGame } = useContext(GameContext);

    return (
        <div className={`p-3 border rounded-full text-center cursor-pointer ${bgWhite}`} onClick={resetGame}>
            Start new game
        </div>
    );
}

export default function InfoPanel() {
    const { gameProgress } = useContext(GameContext);

    return (
        <div className="px-5 pt-20 pb-10 flex flex-col h-full">
            <div className="flex-1">
                {gameProgress === "not started" && <PlayAsPrompt />}
                {gameProgress === "in progress" && <ToMove />}
                {gameProgress === "finished" && <GameResult />}
            </div>
            {gameProgress !== "not started" && <NewGameButton />}
        </div>
    );
}
