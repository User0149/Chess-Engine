export type StateSetter<T> = React.Dispatch<React.SetStateAction<T>>;
export type Coordinate = [row: number, col: number];

export type GameProgress = "in progress" | "not started" | "finished";
export type GameResult = "You win by checkmate" | "Computer wins by checkmate" | "Draw by stalemate" | "Draw by repetition" | "Draw by 50-move rule" | "Draw by insufficient material";

export type PlayerColor = "black" | "white";
export type PieceType = "pawn" | "rook" | "knight" | "bishop" | "queen" | "king";

export interface Piece {
    active: boolean;
    
    color: PlayerColor;
    type: PieceType;
    lastMoveIndex: number;
}

export type BoardState = Piece[][];

export interface GameState {
    moves: number;
    previousStates: Record<string, number>;
    lastCaptureOrPawnMove: number;
    toMove: PlayerColor;

    boardState: BoardState;
}

export type File = "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h";
export type Rank = "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8";

export interface Square {
    file: File;
    rank: Rank;
}

export interface Move {
    source: Square;
    dest: Square;
}

export type PossibleMoves = [Move, GameState][];
