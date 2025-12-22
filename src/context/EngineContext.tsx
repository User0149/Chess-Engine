import { createContext, useEffect, useState, type ReactNode } from "react";

// @ts-ignore
import ModuleFactory from "../engine/engine.mjs";

interface EngineContextProviderProps {
    children: ReactNode;
}

export const EngineContext = createContext({
    isCheckmate: null,
    isStalemate: null,
    threefoldRepetition: null,
    fiftyMoveRule: null,
    insufficientMaterial: null,

    possibleMoves: null,
    computerMove: null
});

export default function EngineContextProvider({ children }: EngineContextProviderProps) {
    const [engine, setEngine] = useState({
        isCheckmate: null,
        isStalemate: null,
        threefoldRepetition: null,
        fiftyMoveRule: null,
        insufficientMaterial: null,

        possibleMoves: null,
        computerMove: null
    });

    useEffect(() => {
        (async () => {
            setEngine(await ModuleFactory());
        })();
    }, []);

    return (
        <EngineContext value={engine}>
            {children}
        </EngineContext>
    );
}
