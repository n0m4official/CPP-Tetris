#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <string>
#include <Windows.h>
#include <queue>
#include <sstream>
#include <regex>
#include "Board.h"
#include "Highscores.h"
#include "ConsoleUtils.h"

using namespace std;

/*
NOTE:
I made this project originally thinking that it would be an easy single script thing.
But now if I move anything else in this file to separate ones, everything stops working.
I am so sorry for the monolith of code in this one file.
I just hope you all enjoy this version I made as I learn how to code in C++ :)
*/

// If you’re looking for clean code, you’re in the wrong place.
// This is Tetris, not a code beauty contest.

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

// Wall kick offsets for pieces J, L, T, S, Z (O doesn't need wall kicks)
// If you understand this on the first try, you deserve a medal.
const vector<pair<int, int>> JLTSZ_WALL_KICKS[4] = {
    {{0,0},{-1,0},{-1,1},{0,-2},{-1,-2}},  // 0->R
    {{0,0},{1,0},{1,-1},{0,2},{1,2}},      // R->2
    {{0,0},{1,0},{1,1},{0,-2},{1,-2}},     // 2->L
    {{0,0},{-1,0},{-1,-1},{0,2},{-1,2}}    // L->0
};

// Wall kick offsets for I piece
// The I piece: because every game needs a troublemaker.
const vector<pair<int, int>> I_WALL_KICKS[4] = {
    {{0,0},{-2,0},{1,0},{-2,-1},{1,2}},  // 0->R
    {{0,0},{-1,0},{2,0},{-1,2},{2,-1}},  // R->2
    {{0,0},{2,0},{-1,0},{2,1},{-1,-2}},  // 2->L
    {{0,0},{1,0},{-2,0},{1,-2},{-2,1}}   // L->0
};

// Load custom colors from config file
// If this fails, you get default colors. Deal with it.
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
// Because everyone deserves a little customization.
void saveColors() {
    ofstream file("colors.cfg");
    for (int i = 0; i < 7; i++) {
        file << COLORS[i] << "\n";
    }
    file.close();
}

// Interactive color customization
// Warning: May cause indecision and existential crisis.
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

// --- Menus ---
// If you’re still reading, you must really love menus.

// Display start menu
// --- Start Menu with Highscores ---
void showStartMenu() {
    system("cls");
    resetCursor();

    string border = "====================================";
    printCentered("\033[1;36m" + border + "\033[0m", 5);
    printCentered("\033[1;36m           TETRIS v1.2.5            \033[0m", 6);
    printCentered("\033[1;36m          By n0m4official           \033[0m", 7);
    printCentered("\033[1;36m" + border + "\033[0m", 8);

    // --- Controls ---
    printCentered("\033[1;33mControls:\033[0m", 10);
    printCentered("A / D : Move Left / Right", 11);
    printCentered("W     : Rotate", 12);
    printCentered("C     : Hold piece", 13);
    printCentered("S     : Soft Drop", 14);
    printCentered("Space : Hard Drop", 15);

    printCentered("\033[1;33mC: Customize Colors\033[0m", 17);
    printCentered("\033[1;33mQ: Quit\033[0m", 18);

    // --- Load and display highscores ---
    std::vector<HighscoreEntry> highscores = loadHighscores("highscores.txt");
    printCentered("\033[1;36m=== HIGHSCORES ===\033[0m", 20);
    int y = 21;
    int rank = 1;
    for (const auto& entry : highscores) {
        printCentered(std::to_string(rank) + ". " + entry.name + " - " + std::to_string(entry.score), y++);
        rank++;
        if (rank > 10) break; // show top 10
    }

    // --- Prompt to start ---
    string prompt = "\033[1;32mPress ENTER to start game...\033[0m";
    int promptY = y + 1;
    while (true) {
        if (_kbhit()) {
            char c = _getch();
            if (c == '\r') break; // start game
            if (c == 'c' || c == 'C') {
                customizeColors();
                system("cls");
                showStartMenu(); // restart menu after customizing
                return;
            }
            if (c == 'q' || c == 'Q') exit(0);
        }
        printCentered(prompt, promptY);
        Sleep(500);
        printCentered(string(visibleLength(prompt), ' '), promptY); // blinking effect
        Sleep(500);
    }

    system("cls");
    resetCursor();
}

// Display exit menu (when user quits mid-game)
// If you quit, I won’t judge. Much.
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
        maxLen = std::max(maxLen, static_cast<size_t>(visibleLength(l)));
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
            system("cls");
            showStartMenu();
            return;
        }
        Sleep(10);
    }
    system("cls");
    resetCursor();
}

// Display game over menu with restart option
#include <limits> // for std::numeric_limits

bool showGameOverMenu(int score, int lines, int level) {
    system("cls");
    resetCursor();

    // Ask player for name
    // If you enter "AAA", you’re officially a retro gamer.
    std::string playerName;
    printCentered("GAME OVER", 5);
    printCentered("Final Score: " + intToString(score), 7);
    printCentered("Enter your name (max 10 chars): ", 9);

    std::cin >> playerName;
    if (playerName.length() > 10)
        playerName = playerName.substr(0, 10);

    // Load, update, save highscores
    std::vector<HighscoreEntry> highscores = loadHighscores("highscores.txt");
    addHighscore(highscores, { playerName, score });
    saveHighscores(highscores, "highscores.txt");

    // Show updated highscores
    showHighscores(highscores, playerName);

    // Ask to restart or quit
    printCentered("Press R to Restart or Q to Quit...", 16);

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
// Why did I have to choose to do binary literals?
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
// If you can rotate yourself this easily, you’re probably a gymnast.
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
    int rotation;
    // If you change this, the Tetris gods will be angry.
};

// Spawn a new piece at the top center
// Like a fresh start, but with more anxiety.
Piece spawnPiece(int id) {
    return Piece{
        TETROMINOES[id], WIDTH / 2 - 2, 0, id
    };
}

// Calculate ghost piece Y position
// Ghosts: because every game needs a haunting.
int getGhostY(const Board& board, const Piece& current) {
    int y = current.y;
    while (board.isValidPosition(current.shape, current.x, y + 1)) {
        y++;
    }
    return y;
}

// Calculate hard drop distance
// If only life had a hard drop button.
int dropDistance(const Board& board, const Piece& piece) {
    int dist = 0;
    while (board.isValidPosition(piece.shape, piece.x, piece.y + dist + 1)) {
        dist++;
    }
    return dist;
}

/*
Disocvered a bug where sometimes pieces teleport when specific conditions are met.
Unsure of cause but assumed to be something in tryRotateSRS function.
Also replicating it is extremely difficult.
*/
// If you fix this bug, you get a free cookie.
bool tryRotateSRS(Piece& piece, const Board& board) {
    int prevRot = piece.rotation;
    int nextRot = (piece.rotation + 1) % 4;

    auto rotated = rotateCW(piece.shape);

    const vector<pair<int, int>>* kicks;
    if (piece.id == 0) // I-piece
        kicks = I_WALL_KICKS;
    else if (piece.id == 1) // O-piece
        return true; // O-piece doesn't need kicks, just rotate in place
    else
        kicks = JLTSZ_WALL_KICKS;

    // Try all wall kick offsets
    for (auto [dx, dy] : kicks[prevRot]) {
        int newX = piece.x + dx;
        int newY = piece.y + dy;
        if (board.isValidPosition(rotated, newX, newY)) {
            piece.shape = rotated;
            piece.x = newX;
            piece.y = newY;
            piece.rotation = nextRot;
            return true;
        }
    }
    return false; // rotation failed even after kicks
}



// --- Rendering Functions ---
// Draw board, current piece, and ghost piece
// If you see weird colors, it’s not a bug, it’s a feature.
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
const int QUEUE_SIZE = 3;
std::queue<Piece> nextQueue; // global queue

// This thing took me WAAAAAAAAAAY too long to get working.
// If you break it, you fix it.
void printNextQueue(const std::queue<Piece>& q) { // pass by const reference
    std::queue<Piece> copy = q; // local copy

    int consoleWidth = getConsoleWidth();
    int boxWidth = 8;
    int offsetX = max(0, (consoleWidth - WIDTH * 2 - boxWidth) / 2 + WIDTH + 20);
    int verticalOffset = 3;

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    for (int i = 0; i < QUEUE_SIZE && !copy.empty(); i++) {
        Piece p = copy.front();
        copy.pop();

        for (int y = 0; y < 6; y++) {
            COORD pos = { (SHORT)offsetX, (SHORT)(verticalOffset + y + i * 6) };
            SetConsoleCursorPosition(hOut, pos);

            if (y == 0 || y == 5) {
                cout << "+" << string(8, '-') << "+";
            }
            else {
                cout << "|";
                for (int x = 0; x < 4; x++) {
                    if (p.shape[y - 1][x]) {
                        cout << COLORS[p.id] << "[]" << RESET;
                    }
                    else {
                        cout << "  ";
                    }
                }
                cout << "|";
            }
        }
    }
}

Piece hold{ {}, 0, 0, -1 }; // initially empty
bool holdUsed = false;      // can only hold once per piece

// Pause menu overlay
// If you pause, remember to stretch and hydrate.
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

// I have tried several times to stop this from being redrawn after every update, still cannot figure it out as of 03-11-2025.
// If you solve this, you get a gold star.
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
// If you make it through this loop, you deserve a break.
int main() {
    srand((unsigned)time(nullptr));
    loadColors();
    showStartMenu();

    bool restart = true;
    while (restart) {
        Board board;
        int totalLines = 0, score = 0, level = 0;
        const int BASE_INTERVAL = 1000;
        int fallInterval = BASE_INTERVAL;
        clock_t lastFall = clock();

        // --- Initialize nextQueue ---
        while (!nextQueue.empty()) nextQueue.pop();
        for (int i = 0; i < QUEUE_SIZE; i++) {
            nextQueue.push(spawnPiece(rand() % 7));
        }

        Piece current = nextQueue.front(); nextQueue.pop();
        nextQueue.push(spawnPiece(rand() % 7));

        Piece hold;
        hold.id = -1;
        bool holdUsed = false;

        bool dirty = true;
        bool running = true;
        bool paused = false;

        while (running) {
            // --- Input Handling ---
            // If you lose, just blame the controls.
            if (_kbhit()) {
                char cmd = _getch();
                if (!paused) {
                    if (cmd == 'a' && board.isValidPosition(current.shape, current.x - 1, current.y)) { current.x--; dirty = true; }
                    if (cmd == 'd' && board.isValidPosition(current.shape, current.x + 1, current.y)) { current.x++; dirty = true; }
                    if (cmd == 'w') { if (tryRotateSRS(current, board)) dirty = true; }
                    if (cmd == 's') { if (board.isValidPosition(current.shape, current.x, current.y + 1)) { current.y++; dirty = true; } }

                    if (cmd == ' ') { // HARD DROP
                        int dist = dropDistance(board, current);
                        current.y += dist;

                        // Place piece immediately
                        board.placePiece(current.shape, current.x, current.y, current.id);
                        int cleared = board.clearLines();
                        totalLines += cleared;
                        score += board.scoreForLines(cleared, level);
                        holdUsed = false;

                        // Advance queue
                        current = nextQueue.front(); nextQueue.pop();
                        nextQueue.push(spawnPiece(rand() % 7));

                        // Check game over
                        if (!board.isValidPosition(current.shape, current.x, current.y)) {
                            resetCursor();
                            printBoard(board, current, score, totalLines, level);
                            printNextQueue(nextQueue);
                            restart = showGameOverMenu(score, totalLines, level);
                            running = false;
                            break;
                        }

                        dirty = true;
                        lastFall = clock();
                    }
                    if (cmd == 'c' || cmd == 'C') { // HOLD PIECE
                        if (!holdUsed) {
                            if (hold.id == -1) { // first hold
                                hold = current;
                                current = nextQueue.front(); nextQueue.pop();
                                nextQueue.push(spawnPiece(rand() % 7));
                            }
                            else {
                                Piece temp = current;
                                current = hold;
                                hold = temp;
                                current.x = WIDTH / 2 - 2;
                                current.y = 0;
                            }
                            holdUsed = true;
                            dirty = true;
                        }
                    }

                    if (cmd == 'q' || cmd == 'Q') { running = false; showGameExitedMenu(score, totalLines, level); restart = true; break; }
                    if (cmd == 'p' || cmd == 'P') { paused = true; showPauseMenu(board, current, score, totalLines, level); }
                }
                else { // Paused
                    if (cmd == 'p' || cmd == 'P') { paused = false; system("cls"); dirty = true; }
                    if (cmd == 'q' || cmd == 'Q') { running = false; showGameExitedMenu(score, totalLines, level); restart = true; break; }
                }
            }

            // --- Automatic piece fall ---
            // Gravity: not just for apples.
            if (!paused) {
                clock_t now = clock();
                if ((now - lastFall) * 1000 / CLOCKS_PER_SEC >= fallInterval) {
                    lastFall = now;
                    if (board.isValidPosition(current.shape, current.x, current.y + 1)) {
                        current.y++;
                        dirty = true;
                    }
                    else {
                        // Lock piece
                        board.placePiece(current.shape, current.x, current.y, current.id);
                        int cleared = board.clearLines();
                        totalLines += cleared;
                        score += board.scoreForLines(cleared, level);
                        holdUsed = false;

                        // Advance queue
                        current = nextQueue.front(); nextQueue.pop();
                        nextQueue.push(spawnPiece(rand() % 7));

                        // Check game over
                        if (!board.isValidPosition(current.shape, current.x, current.y)) {
                            resetCursor();
                            printBoard(board, current, score, totalLines, level);
                            printNextQueue(nextQueue);
                            restart = showGameOverMenu(score, totalLines, level);
                            running = false;
                            break;
                        }
                        dirty = true;

                        // Update level & fall speed
                        level = totalLines / 10;
                        fallInterval = max(100, BASE_INTERVAL - level * 100);
                    }
                }
            }

            // --- Rendering ---
            // If you see flickering, just squint harder.
            if (dirty && !paused) {
                resetCursor();
                printBoard(board, current, score, totalLines, level);
                printNextQueue(nextQueue);
                printHoldPiece(hold);
                dirty = false;
            }

            Sleep(16); // ~60 FPS
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