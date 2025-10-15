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

// --- Convert integer to string ---
inline string intToString(int n) {
    std::stringstream ss;
    ss << n;
    return ss.str();
}

// --- Strip ANSI color codes for accurate string length ---
inline int visibleLength(const string& s) {
    return static_cast<int>(std::regex_replace(s, std::regex("\033\\[[0-9;]*m"), "").size());
}

// --- Get current console width ---
inline int getConsoleWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int width = 80; // default fallback
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    return width;
}

// --- Reset cursor to top-left ---
inline void resetCursor() {
    COORD pos;
    pos.X = 0;
    pos.Y = 0;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// --- Print text centered horizontally at a specific row ---
inline void printCentered(const string& text, int y) {
    int consoleWidth = getConsoleWidth();
    int x = std::max(0, (consoleWidth - static_cast<int>(visibleLength(text))) / 2);

    COORD pos;
    pos.X = static_cast<SHORT>(x);
    pos.Y = static_cast<SHORT>(y);

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    cout << text;
}
