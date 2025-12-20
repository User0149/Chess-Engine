import { createContext } from "react";

interface IThemeContext {
    blackColor: string;
    whiteColor: string;

    bgWhite: string;
    bgBlack: string;
    bgSelected: string;
}

export const ThemeContext = createContext<IThemeContext>({
    whiteColor: "#ffd59a",
    blackColor: "#b16d40",

    bgWhite: "bg-[#ffd59a]",
    bgBlack: "bg-[#b16d40]",
    bgSelected: "bg-[yellow]"
});
