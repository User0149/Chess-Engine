import { useContext, useState, type ReactNode } from "react";

import type { File, PossibleMove, Rank, StateSetter } from "../types/types";

import { squareToCoord } from "../utils/coordinateConverter";
import { toEngineGameState, toJSPossibleMove } from "../utils/jsEmbindConverter";

import { GameContext } from "../context/GameContext";
import { ThemeContext } from "../context/ThemeContext";
import { EngineContext } from "../context/EngineContext";

interface BoardSquareProps {
    rank: Rank;
    file: File;
    setShowPromotionModal: StateSetter<boolean>;
    setPromotionOptions: StateSetter<PossibleMove[]>;
}

interface PromotionModalProps {
    setShowPromotionModal: StateSetter<boolean>;
    promotionOptions: PossibleMove[];
}

function BoardSquare({ file, rank, setShowPromotionModal, setPromotionOptions }: BoardSquareProps) {
    const { bgWhite, bgBlack, bgSelected } = useContext(ThemeContext);
    const { playerColor, gameProgress, gameState, lastMove, selectedSquare, makeMove, setSelectedSquare } = useContext(GameContext);
    const engine: any = useContext(EngineContext);

    const coordinate = squareToCoord({ file, rank });
    const piece = gameState.boardState[coordinate[0]][coordinate[1]];

    const squareInvolvedInLastMove = gameState.moves >= 1 && ((lastMove.source.file === file && lastMove.source.rank === rank) || (lastMove.dest.file === file && lastMove.dest.rank === rank));
    const isSelected = selectedSquare?.file === file && selectedSquare?.rank === rank;

    const playersTurn = gameProgress === "in progress" && (gameState.toMove === playerColor);

    const handleSquareClick = () => {
        // only handle clicks during player's move
        if (!playersTurn) return;

        const piece = gameState.boardState[coordinate[0]][coordinate[1]];
        
        if (piece.active && piece.color === playerColor) {
            // player clicked on their own piece

            if (selectedSquare?.file === file && selectedSquare?.rank === rank) {
                // player wants to undo selection by clicking on the same square
                setSelectedSquare(null);
            }
            else {
                // select the piece
                setSelectedSquare({ file, rank });
            }
        }
        else {
            // player clicked on a square not containing their piece
            if (selectedSquare !== null) {
                // a piece has previously been selected: try to move the piece to this square

                // obtain all legal possible moves that correspond to the player's move
                const moveOptions: PossibleMove[] = [];
                const possibleMovesArray = engine.possibleMoves(toEngineGameState(engine, gameState));

                for (let i = 0; i < possibleMovesArray.size(); i++) {
                    const possibleMove = possibleMovesArray.get(i);
                    if (possibleMove.move.source.file === selectedSquare.file && possibleMove.move.source.rank === selectedSquare.rank &&
                        possibleMove.move.dest.file === file && possibleMove.move.dest.rank === rank) {
                        // this is a legal move
                        moveOptions.push(toJSPossibleMove(possibleMove));
                    }
                }

                // do the move, if legal
                if (moveOptions.length === 0) { // illegal move
                    setSelectedSquare(null);
                }
                else if (moveOptions.length === 1) { // non-promotion move
                    makeMove(moveOptions[0]);
                    setSelectedSquare(null);
                }
                else if (moveOptions.length === 4) { // promotion
                    setShowPromotionModal(true);
                    setPromotionOptions(moveOptions);
                }
                else {
                    setSelectedSquare(null);
                    throw new Error("Error: what is this move");
                }
            }
        }
    };

    return (
        <div className={`aspect-square relative ${(coordinate[0] % 2 === coordinate[1] % 2) ? bgBlack : bgWhite}`}
            onClick={handleSquareClick}
        >
            {
                piece.active && 
                <div className={`h-full w-full flex items-center justify-center select-none ${playersTurn ? "cursor-pointer" : "cursor-not-allowed"}`}>
                    <img src={`./assets/chess-pieces/${piece.type}-${piece.color}.svg`} className="w-4/5 aspect-square z-100" />
                </div>
            }
            {
                (squareInvolvedInLastMove || isSelected) && <div className={`absolute inset-0 opacity-50 ${bgSelected} h-full w-full`}></div>
            }
        </div>
    );
}

function PromotionModal({ setShowPromotionModal, promotionOptions }: PromotionModalProps) {
    const { playerColor, setSelectedSquare, makeMove } = useContext(GameContext);

    return (
        <div className="absolute inset-0 flex flex-col items-center pt-30 h-full w-full bg-black/50 z-1000" onClick={() => {
            setShowPromotionModal(false);
            setSelectedSquare(null);
        }}>
            <div className="text-white text-2xl p-4">
                Promote to
            </div>
            <div className="grid grid-cols-2">
                {
                    promotionOptions.map(possibleMove => {
                        const pieceType = possibleMove.move.newPieceType;
                        return (
                            <img key={pieceType} src={`./assets/chess-pieces/${pieceType}-${playerColor}.svg`} className="w-25 h-25 p-2 cursor-pointer" onClick={() => {
                                makeMove(possibleMove);
                                setShowPromotionModal(false);
                                setSelectedSquare(null);
                            }} />
                        );
                    })
                }
            </div>
        </div>
    );    
}

export default function Board() {
    const { playerColor, gameProgress } = useContext(GameContext);

    const [showPromotionModal, setShowPromotionModal] = useState(false);
    const [promotionOptions, setPromotionOptions] = useState<PossibleMove[]>([]);

    const Squares: ReactNode[] = [];

    const ranks: Rank[] = ["8", "7", "6", "5", "4", "3", "2", "1"];
    const files: File[] = ["a", "b", "c", "d", "e", "f", "g", "h"];

    if (playerColor === "black") {
        ranks.reverse();
        files.reverse();
    }

    for (const rank of ranks) {
        for (const file of files) {
            Squares.push(
                <BoardSquare key={`${file}${rank}`} file={file} rank={rank} setShowPromotionModal={setShowPromotionModal} setPromotionOptions={setPromotionOptions} />
            );
        }
    }

    return (
        <div className="h-full w-full p-4">
            <div className="h-full aspect-square relative">
                <div className="grid grid-cols-8">
                    {Squares}
                </div>

                {gameProgress !== "in progress" && <div className="absolute inset-0 h-full w-full bg-black opacity-20 z-1000"></div>}
                {showPromotionModal && <PromotionModal promotionOptions={promotionOptions} setShowPromotionModal={setShowPromotionModal} />}
            </div>
        </div>
    );
}
