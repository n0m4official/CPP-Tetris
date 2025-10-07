#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <string>
#include <Windows.h>
#include <sstream>
#include <regex>
#include "Board.h"

using namespace std;

// --- Constants ---
// Standard Tetris board size of 10x20
const int WIDTH = 10;   ///< Width of the Tetris board
const int HEIGHT = 20;  ///< Height of the Tetris board

// ANSI escape codes for colors (Windows ANSI support)
const string RESET = "\033[0m";
string COLORS[7] = {
    "\033[36m",
    "\033[33m",
    "\033[35m",
    "\033[32m",
    "\033[34m",
    "\033[31m",
    "\033[91m"
};

// --- Utility Functions ---

// Load custom colors from config file
void loadColors() {
    ifstream file("colors.cfg");
    if (!file.is_open()) return; // First run, file doesn't exist

    string line;
    int i = 0;
    while (getline(file, line) && 1 < 7) {
        COLORS[i] = line;
        i++;
    }
    file.close();
}

// Save custom colors to config file
void saveColors() {
    ofstream file("colors.cfg");
    for (int i = 0; i < 7; i++) {
        file << COLORS[i] << "\n";
    }
    file.close();
}

// Interactive color customization
void customizeColors() {
    string names[7] = { "Cyan", "Yellow", "Magenta", "Green", "Blue", "Red", "Bright Red" };
    string codes[7] = { "\033[36m","\033[33m","\033[35m","\033[32m","\033[34m","\033[31m","\033[91m" };

    for (int i = 0; i < 7; i++) {
        cout << "Choose color for piece " << i << " (" << names[i] << "):\n";
        for (int j = 0; j < 7; j++) {
            cout << j << ": " << codes[j] << "[]" << RESET << " " << names[j] << "\n";
        }
        int choice = 0;
        cin >> choice;
        if (choice >= 0 && choice < 7) COLORS[i] = codes[choice];
    }
    saveColors();
}

// Convert integer to string (safe alternative to std::to_string)
string intToString(int n) {
    stringstream ss;
    ss << n;
    return ss.str();
}

// Strip ANSI color codes for accurate string length
int visibleLength(const string& s) {
    return regex_replace(s, regex("\033\\[[0-9;]*m"), "").size();
}

// Get current colsole width for centered UI
int getConsoleWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int width = 80;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    return width;
}

// Reset cursor back to top-left corner
void resetCursor() {
    COORD pos = { 0, 0 };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// Print text centered horizontally at specific row
void printCentered(const string& text, int y) {
    int consoleWidth = getConsoleWidth();
    int x = max(0, (consoleWidth - (int)visibleLength(text)) / 2);
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    cout << text;
}

// --- Menus ---

// Display start menu
void showStartMenu() {
    system("cls");
    resetCursor();

    string border = "====================================";
    printCentered("\033[1;36m" + border + "\033[0m", 5);
    printCentered("\033[1;36m           TETRIS v1.1.5            \033[0m", 6);
    printCentered("\033[1;36m          By n0m4official           \033[0m", 7);
    printCentered("\033[1;36m" + border + "\033[0m", 8);

    printCentered("\033[1;33mControls:\033[0m", 10);
    printCentered("A / D : Move Left / Right", 11);
    printCentered("W     : Rotate", 12);
    printCentered("C     : Hold piece", 13);
    printCentered("S     : Soft Drop", 14);
    printCentered("Space : Hard Drop", 15);
    printCentered("Q     : Quit", 16);

    string prompt = "\033[1;32mPress ENTER to start...\033[0m";
    int y = 18;
    while (true) {
        if (_kbhit() && _getch() == '\r') {
            break;
        }
        printCentered(prompt, y);
        Sleep(500);
        printCentered(string(visibleLength(prompt), ' '), y); // clear
        Sleep(500);
    }

    system("cls");
    resetCursor();
}

// Display exit menu (when user quits mid-game)
void showGameExitedMenu(int score, int lines, int level) {
    system("cls");
    resetCursor();

    vector<string> linesToPrint = {
        "GAME EXITED",
        "Final Score   : " + intToString(score),
        "Lines Cleared : " + intToString(lines),
        "Level Reached : " + intToString(level),
        "Press ENTER to return to start menu..."
    };

    size_t maxLen = 0;
    for (const auto& l : linesToPrint) {
        maxLen = max(maxLen, visibleLength(l));
    }
    string border(maxLen + 4, '=');

    printCentered("\033[1;31m" + border + "\033[0m", 5);
    printCentered("\033[1;31m  " + linesToPrint[0] + "  \033[0m", 6);
    printCentered("\033[1;31m" + border + "\033[0m", 7);

    printCentered("\033[1;33m" + linesToPrint[1] + "\033[0m", 9);
    printCentered("\033[1;33m" + linesToPrint[2] + "\033[0m", 10);
    printCentered("\033[1;33m" + linesToPrint[3] + "\033[0m", 11);

    printCentered("\033[1;32m" + linesToPrint[4] + "\033[0m", 13);

    while (true) {
        if (_kbhit() && _getch() == '\r') {
            break;
        }
        Sleep(10);
    }

    system("cls");
    resetCursor();
}

// Display game over menu with restart option
bool showGameOverMenu(int score, int lines, int level) {
    system("cls");      
    resetCursor();

    // Array for menu display
    vector<string> linesToPrint = {
        "GAME OVER",
        "Final Score   : " + intToString(score),
        "Lines Cleared : " + intToString(lines),
        "Level Reached : " + intToString(level),
        "Press R to Restart or Q to Quit..."
    };

    size_t maxLen = 0;
    for (const auto& l : linesToPrint) {
        maxLen = max(maxLen, visibleLength(l));
    }
    string border(maxLen + 4, '=');

    printCentered("\033[1;31m" + border + "\033[0m", 5);
    printCentered("\033[1;31m  " + linesToPrint[0] + "  \033[0m", 6);
    printCentered("\033[1;31m" + border + "\033[0m", 7);

    printCentered("\033[1;33m" + linesToPrint[1] + "\033[0m", 9);
    printCentered("\033[1;33m" + linesToPrint[2] + "\033[0m", 10);
    printCentered("\033[1;33m" + linesToPrint[3] + "\033[0m", 11);
    printCentered("\033[1;32m" + linesToPrint[4] + "\033[0m", 13);

    while (true) {
        if (_kbhit()) {
            char c = _getch();
            if (c == 'r' || c == 'R') {
                system("cls");
                resetCursor();
                return true;
            }
            if (c == 'q' || c == 'Q') {
                system("cls");
                resetCursor();
                return false;
            }
        }
        Sleep(10);
    }
}

// --- Tetromino Logic ---

// Standard Tetris pieces (7 pieces, 4x4 matrices)
vector<vector<vector<int>>> TETROMINOES = {
    {   // I
        {0,0,0,0},
        {1,1,1,1},
        {0,0,0,0},
        {0,0,0,0}
    },
    {   // O
        {0,0,0,0},
        {0,1,1,0},
        {0,1,1,0},
        {0,0,0,0}
    },
    {   // T
        {0,1,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    {   // J
        {0,0,0,0},
        {0,0,1,0},
        {1,1,1,0},
        {0,0,0,0}
    },
    {   // L
        {0,0,0,0},
        {1,0,0,0},
        {1,1,1,0},
        {0,0,0,0}
    },
    {   // S
        {0,0,0,0},
        {0,1,1,0},
        {1,1,0,0},
        {0,0,0,0}
    },
    {   // Z
        {0,0,0,0},
        {1,1,0,0},
        {0,1,1,0},
        {0,0,0,0}
    }
};


// Rotate a piece 90 degrees clockwise
vector<vector<int>> rotateCW(const vector<vector<int>>& shape) {
    int n = (int)shape.size();
    vector<vector<int>> r(n, vector<int>(n, 0));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            r[j][n - 1 - i] = shape[i][j];
        }
    }
    return r;
}

// Piece representation
struct Piece {
    vector<vector<int>> shape;  ///< 4x4 matrix
    int x, y;                   ///< Position on board
    int id;                     ///< Piece type/color
};

// Spawn a new piece at the top center
Piece spawnPiece(int id) {
    return Piece{
        TETROMINOES[id], WIDTH / 2 - 2, 0, id
    };
}

// Calculate ghost piece Y position
int getGhostY(const Board& board, const Piece& current) {
    int y = current.y;
    while (board.isValidPosition(current.shape, current.x, y + 1)) {
        y++;
    }
    return y;
}

// Calculate hard drop distance
int dropDistance(const Board& board, const Piece& piece) {
    int dist = 0;
    while (board.isValidPosition(piece.shape, piece.x, piece.y + dist + 1)) {
        dist++;
    }
    return dist;
}


// --- Rendering Functions ---

// Draw board, current piece, and ghost piece
void printBoard(const Board& board, const Piece& current, int score, int lines, int level) {
    int consoleWidth = getConsoleWidth();
    int boardWidth = WIDTH * 2 + 2;
    int offsetX = max(0, (consoleWidth - boardWidth) / 2);

    COORD pos = { 0, 0 };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

    cout << string(offsetX, ' ') << "Score: " << score << "  Lines: " << lines << "  Level: " << level << "\n\n";
    cout << string(offsetX, ' ') << "+" << string(WIDTH * 2, '-') << "+\n";

    int ghostY = getGhostY(board, current);
    for (int y = 0; y < HEIGHT; ++y) {
        cout << string(offsetX, ' ') << "|";
        for (int x = 0; x < WIDTH; ++x) {
            bool occupied = board.grid[y][x] != 0;
            int pieceId = occupied ? board.grid[y][x] - 1 : -1;
            int sx = x - current.x, sy = y - current.y;

            if (sx >= 0 && sx < (int)current.shape[0].size() && sy >= 0 && sy < (int)current.shape.size() && current.shape[sy][sx]) {
                cout << COLORS[current.id] << "[]" << RESET;
            }
            else if (!occupied && y >= ghostY && y < ghostY + (int)current.shape.size() && sx >= 0 && sx < (int)current.shape[0].size() && current.shape[y - ghostY][sx]) {
                cout << "\033[90m" << "[]" << RESET; // ghost piece
            }
            else if (occupied) {
                cout << COLORS[pieceId] << "[]" << RESET;
            }
            else {
                cout << "  ";
            }
        }
        cout << "|\n";
    }
    cout << string(offsetX, ' ') << "+" << string(WIDTH * 2, '-') << "+\n";
}

// Draw next piece preview
void printNextPiece(const Piece& next) {
    int consoleWidth = getConsoleWidth();
    int boxWidth = 8; // 4 blocks * 2
    int offsetX = max(0, (consoleWidth - WIDTH * 2 - boxWidth) / 2 + WIDTH * 2 + 4);

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    int verticalOffset = 3;

    for (int y = 0; y < 6; ++y) {
        COORD pos = { (SHORT)offsetX, (SHORT)(y + verticalOffset) };
        SetConsoleCursorPosition(hOut, pos);

        if (y == 0 || y == 5) {
            cout << "+" << string(8, '-') << "+";
        }
        else {
            cout << "|";
            for (int x = 0; x < 4; ++x) {
                if (next.shape[y - 1][x]) {
                    cout << COLORS[next.id] << "[]" << RESET;
                }
                else {
                    cout << "  ";
                }
            }
            cout << "|";
        }
    }
}

Piece hold{ {}, 0, 0, -1 }; // initially empty
bool holdUsed = false;      // can only hold once per piece

// Pause menu overlay
void showPauseMenu(const Board& board, const Piece& current, int score, int lines, int level) {
    // Redraw board in dim mode
    int consoleWidth = getConsoleWidth();
    int boardWidth = WIDTH * 2 + 2;
    int offsetX = max(0, (consoleWidth - boardWidth) / 2);

    resetCursor();
    cout << string(offsetX, ' ') << "Score: " << score << "  Lines: " << lines << "  Level: " << level << "\n\n";
    cout << string(offsetX, ' ') << "+" << string(WIDTH * 2, '-') << "+\n";

    int ghostY = getGhostY(board, current);
    for (int y = 0; y < HEIGHT; ++y) {
        cout << string(offsetX, ' ') << "|";
        for (int x = 0; x < WIDTH; ++x) {
            bool occupied = board.grid[y][x] != 0;
            int pieceId = occupied ? board.grid[y][x] - 1 : -1;
            int sx = x - current.x, sy = y - current.y;

            if (sx >= 0 && sx < (int)current.shape[0].size() &&
                sy >= 0 && sy < (int)current.shape.size() &&
                current.shape[sy][sx]) {
                cout << "\033[90m[]" << RESET; // dim current piece
            }
            else if (!occupied && y >= ghostY && y < ghostY + (int)current.shape.size() &&
                sx >= 0 && sx < (int)current.shape[0].size() && current.shape[y - ghostY][sx]) {
                cout << "\033[90m[]" << RESET; // dim ghost
            }
            else if (occupied) {
                cout << "\033[90m[]" << RESET; // dim placed block
            }
            else {
                cout << "  ";
            }
        }
        cout << "|\n";
    }
    cout << string(offsetX, ' ') << "+" << string(WIDTH * 2, '-') << "+\n";

    // Overlay text
    string pauseText = "\033[1;36m=== PAUSED ===\033[0m";
    string resumeText = "\033[1;33mPress P to Resume or Q to Quit\033[0m";

    int y = HEIGHT / 2 - 1;
    printCentered(pauseText, y);
    printCentered(resumeText, y + 2);
}

// Draw held piece
void printHoldPiece(const Piece& hold) {
    int consoleWidth = getConsoleWidth();
    int boxWidth = 8; // 4 blocks * 2
    int offsetX = max(0, (consoleWidth - WIDTH * 2 - boxWidth) / 2 - boxWidth - 4);

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    int verticalOffset = 3;

    for (int y = 0; y < 6; ++y) {
        COORD pos = { (SHORT)offsetX, (SHORT)(y + verticalOffset) };
        SetConsoleCursorPosition(hOut, pos);

        if (y == 0 || y == 5) {
            cout << "+" << string(8, '-') << "+";
        }
        else {
            cout << "|";
            for (int x = 0; x < 4; ++x) {
                if (hold.id != -1 && hold.shape[y - 1][x]) {
                    cout << COLORS[hold.id] << "[]" << RESET;
                }
                else {
                    cout << "  ";
                }
            }
            cout << "|";
        }
    }
}

// --- Main Game Loop ---
int main() {
    srand((unsigned)time(nullptr));
    loadColors();
    showStartMenu();
    // TODO: Add a color customization menu

    bool restart = true;
    while (restart) {
        Board board;
        int totalLines = 0, score = 0, level = 0;
        const int BASE_INTERVAL = 1000;
        int fallInterval = BASE_INTERVAL;
        clock_t lastFall = clock();

        Piece current = spawnPiece(rand() % 7);
        Piece next = spawnPiece(rand() % 7);

        Piece hold;
        hold.id = -1;
        bool holdUsed = false;

        bool dirty = true;
        bool running = true;
        bool paused = false;

        while (running) {
            if (_kbhit()) {
                char cmd = _getch();
                if (!paused) {
                    if (cmd == 'a' && board.isValidPosition(current.shape, current.x - 1, current.y)) {
                        current.x--; dirty = true;
                    }
                    if (cmd == 'd' && board.isValidPosition(current.shape, current.x + 1, current.y)) {
                        current.x++; dirty = true;
                    }
                    if (cmd == 'w') {
                        auto r = rotateCW(current.shape);
                        if (board.isValidPosition(r, current.x, current.y)) {
                            current.shape = r; dirty = true;
                        }
                        // TODO: Implement wall kicks for rotation near walls
                    }
                    if (cmd == 's') {
                        if (board.isValidPosition(current.shape, current.x, current.y + 1)) {
                            current.y++; dirty = true;
                        }
                    }
                    if (cmd == ' ') {
                        int dist = dropDistance(board, current);
                        current.y += dist;
                        dirty = true;
                        lastFall = clock() - fallInterval;
                    }
                    if (cmd == 'q' || cmd == 'Q') {
                        running = false;
                        showGameExitedMenu(score, totalLines, level);
                        restart = true;
                        break;
                    }
                    if (cmd == 'p' || cmd == 'P') {
                        paused = true;
                        showPauseMenu(board, current, score, totalLines, level);
                    }

                    if (cmd == 'c' || cmd == 'C') { // Hold piece
                        if (!holdUsed) {
                            if (hold.id == -1) {            // first time holding
                                hold = current;             // move current piece to hold
                                current = next;             // next piece becomes current
                                next = spawnPiece(rand() % 7);
                            }
                            else {
                                // Manual swap of pieces
                                Piece temp = current;
                                current = hold;
                                hold = temp;

                                // Reset current piece position after swap
                                current.x = WIDTH / 2 - 2;
                                current.y = 0;
                            }
                            holdUsed = true; // prevent multiple holds before piece locks
                            dirty = true;    // redraw board
                        }
                    }
                }
                else {
                    // While paused
                    if (cmd == 'p' || cmd == 'P') {
                        paused = false;
                        system("cls");
                        dirty = true; // refresh board on resume
                    }
                    if (cmd == 'q' || cmd == 'Q') {
                        running = false;
                        showGameExitedMenu(score, totalLines, level);
                        restart = true;
                        break;
                    }
                }
            }

            if (!paused) {
                clock_t now = clock();
                if ((now - lastFall) * 1000 / CLOCKS_PER_SEC >= fallInterval) {
                    lastFall = now;
                    if (board.isValidPosition(current.shape, current.x, current.y + 1)) {
                        current.y++;
                        dirty = true;
                    }
                    else {
                        board.placePiece(current.shape, current.x, current.y, current.id);
                        int cleared = board.clearLines();
                        holdUsed = false;

                        if (cleared) {
                            totalLines += cleared;
                            score += board.scoreForLines(cleared, level);
                        }

                        level = totalLines / 10;
                        fallInterval = max(100, BASE_INTERVAL - level * 100);

                        current = next;
                        next = spawnPiece(rand() % 7);
                        dirty = true;

                        if (!board.isValidPosition(current.shape, current.x, current.y)) {
                            resetCursor();
                            printBoard(board, current, score, totalLines, level);
                            printNextPiece(next);
                            restart = showGameOverMenu(score, totalLines, level);
                            break;
                        }
                    }
                }
            }

            if (dirty && !paused) {
                resetCursor();
                printBoard(board, current, score, totalLines, level);
                printNextPiece(next);
                printHoldPiece(hold);
                dirty = false;
            }
            Sleep(16);
        }
        if (!running) {
            break;
        }
    }
    return 0;
}

// --- End Notes ---
// I have a love/hate relationship with C++...
// This was a fun project, but also incredibly frustrating at times.
// I hope you enjoyed reading through my code as much as I enjoyed writing it (mostly).
// Thanks for sticking with me to the end!
// - n0m4official
// --- End of file ---