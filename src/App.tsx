import GameContextProvider from "./context/GameContext";

import Board from "./components/Board";
import InfoPanel from "./components/InfoPanel";

function App() {
    return (
        <GameContextProvider>
            <div className="flex h-full">
                <div className="flex items-center h-full">
                    <Board />
                </div>
                <div className="flex-1">
                    <InfoPanel />
                </div>
            </div>
        </GameContextProvider>
    )
}

export default App;
