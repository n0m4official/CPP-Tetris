#pragma once

#define NOMINMAX // prevent Windows macro conflicts
#include <windows.h>
#undef max // make sure Windows' max macro doesn't interfere

#include <iostream>
#include <sstream>
#include <string>
#include <regex>
#include <algorithm> // std::max

using std::string;
using std::cout;

// If you’re reading this, you’re probably trying to fix a bug.
// Good luck. May the console gods be with you.

// --- Convert integer to string ---
// Because std::to_string was too mainstream for me.
inline string intToString(int n) {
    std::stringstream ss;
    ss << n;
    return ss.str();
}

// --- Strip ANSI color codes for accurate string length ---
// Because colors are pretty, but math is ugly.
inline int visibleLength(const string& s) {
    return static_cast<int>(std::regex_replace(s, std::regex("\033\\[[0-9;]*m"), "").size());
}

// --- Get current console width ---
// If this returns 80, blame Windows. Or Bill Gates.
inline int getConsoleWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int width = 80; // default fallback, like my fallback career in interpretive dance
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    return width;
}

// --- Reset cursor to top-left ---
// Because sometimes you just need a fresh start.
inline void resetCursor() {
    COORD pos;
    pos.X = 0;
    pos.Y = 0;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// --- Print text centered horizontally at a specific row ---
// Centering text: for when you want your game to look fancier than your code.
inline void printCentered(const string& text, int y) {
    int consoleWidth = getConsoleWidth();
    int x = std::max(0, (consoleWidth - static_cast<int>(visibleLength(text))) / 2);

    COORD pos;
    pos.X = static_cast<SHORT>(x);
    pos.Y = static_cast<SHORT>(y);

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    cout << text;
}
