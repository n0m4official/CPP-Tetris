#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <string>
#include <Windows.h>
#include <sstream>
#include <regex>

// NOTE: Windows-only implementation due to _kbhit(), _getch(), and Windows console API

using namespace std;

const int WIDTH = 10;
const int HEIGHT = 20;

// --- Colors ---
// ANSI escape codes for colors
// Funny number make color, I guess...
const string RESET = "\033[0m";
const string COLORS[7] = {
    "\033[36m",
    "\033[33m",
    "\033[35m",
    "\033[32m",
    "\033[34m",
    "\033[31m",
    "\033[91m"
};

// Utility to convert int to string (std::to_string broke on me too many times)
string intToString(int n) {
    stringstream ss;
    ss << n;
    return ss.str();
}

// Strip ANSI codes for accurate length (alighnments were cursed without this)
int visibleLength(const string& s) {
    return regex_replace(s, regex("\033\\[[0-9;]*m"), "").size();
}

// Console width detection (Windows console API sorcery)
int getConsoleWidth() { // Console width calculated once per frame to center board and UI
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int width = 80;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    return width;
}

// Reset cursor back to top left (avoids flickering madness... mostly)
void resetCursor() {
    COORD pos = { 0, 0 };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// Print text centered on X, with Y offset
void printCentered(const string& text, int y) {
    int consoleWidth = getConsoleWidth();
    int x = max(0, (consoleWidth - (int)visibleLength(text)) / 2);
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    cout << text;
}

// --- Start menu ---
// This was way more annoying to make than it had any right to be
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
        printCentered(string(visibleLength(prompt), ' '), y); // erase
        Sleep(500);
    }

    system("cls");
    resetCursor();
}

// --- Game exited menu ---
// Basically the ragequit screen
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

// --- Game over menu ---
// I stared at this for 3 hours straight fixing spacing bugs. Never again.
bool showGameOverMenu(int score, int lines, int level) {
    system("cls");
    resetCursor();

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


// --- Board & logic ---
// Handling the board state and line clearing.
// This class ate two days of my life. I hate line clearing logic.
// It works now though, so whatever.
class Board {
public:
    vector<vector<int>> grid;
    Board() : grid(HEIGHT, vector<int>(WIDTH, 0)) {}

    bool isValidPosition(const vector<vector<int>>& shape, int x, int y) const {
        int h = (int)shape.size();
        int w = (int)shape[0].size();
        for (int i = 0; i < h; ++i) {
            for (int j = 0; j < w; ++j) {
                if (shape[i][j]) {
                    int nx = x + j, ny = y + i;
                    if (nx < 0 || nx >= WIDTH || ny < 0 || ny >= HEIGHT) {
                        return false;
                    }
                    if (grid[ny][nx] != 0) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    void placePiece(const vector<vector<int>>& shape, int x, int y, int id) {
        for (int i = 0; i < (int)shape.size(); ++i) {
            for (int j = 0; j < (int)shape[0].size(); ++j) {
                if (shape[i][j]) {
                    grid[y + i][x + j] = id + 1;
                }
            }
        }
    }

    int clearLines() {
        int cleared = 0;
        for (int row = HEIGHT - 1; row >= 0; --row) {
            if (all_of(grid[row].begin(), grid[row].end(), [](int c) { return c != 0; })) {
                grid.erase(grid.begin() + row);
                grid.insert(grid.begin(), vector<int>(WIDTH, 0));
                ++cleared;
                ++row;
            }
        }
        return cleared;
    }

    int scoreForLines(int lines, int level) {
        switch (lines) {
        case 1: return 40 * (level + 1);
        case 2: return 100 * (level + 1);
        case 3: return 300 * (level + 1);
        case 4: return 1200 * (level + 1);
        default: return 0;
        }
    }
};

// --- Tetrominoes ---
// 7 standard Tetris pieces in 4x4 matrices
// 0 = empty, 1 = filled
// Indexed 0-6 for easy random selection
// Took me way too long to get these right, I hate binary literals...
vector<vector<vector<int>>> TETROMINOES = {
    {
        {0,0,0,0},
        {1,1,1,1},
        {0,0,0,0},
        {0,0,0,0}
    },
    {
        {0,0,0,0},
        {0,1,1,0},
        {0,1,1,0},
        {0,0,0,0}
    },
    {
        {0,1,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    {
        {0,0,0,0},
        {0,0,1,0},
        {1,1,1,0},
        {0,0,0,0}
    },
    {
        {0,0,0,0},
        {1,0,0,0},
        {1,1,1,0},
        {0,0,0,0}
    },
    {
        {0,0,0,0},
        {0,1,1,0},
        {1,1,0,0},
        {0,0,0,0}
    },
    {
        {0,0,0,0},
        {1,1,0,0},
        {0,1,1,0},
        {0,0,0,0}
    }
};

// --- Rotation ---
// Rotate a piece 90 degrees clockwise
// This was incredibly annoying to get it to work properly
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

// --- Piece spawning & ghost piece calculation ---
// Piece struct to hold shape, position, and ID
// Excessive for this simple game, but whatever, was fun to make
struct Piece {
    vector<vector<int>> shape;
    int x, y, id;
};

// Spawn a new piece at the top center
// ID is used for color and shape selection
// Starts at y=0, x=centered
// Why was this so hard to get to work?
Piece spawnPiece(int id) {
    return Piece{
        TETROMINOES[id], WIDTH / 2 - 2, 0, id
    };
}

// Calculate ghost piece Y position
// Ghost piece shows where the current piece would land if dropped
// Does not modify the piece, just calculates the position
// Do not ask me why this was so hard to get right, I have no idea
// It just... was.
int getGhostY(const Board& board, const Piece& current) {
    int y = current.y;
    while (board.isValidPosition(current.shape, current.x, y + 1)) {
        y++;
    }
    return y;
}

// Calculate drop distance for hard drop
// Returns how many rows the piece can drop
// Used for hard drop functionality
// Some was easy after I got ghost piece working
int dropDistance(const Board& board, const Piece& piece) {
    int dist = 0;
    while (board.isValidPosition(piece.shape, piece.x, piece.y + dist + 1)) {
        dist++;
    }
    return dist;
}

// --- Draw board centered ---
// Draws the board with current piece and ghost piece
// Centers the board in the console
// Uses ANSI colors for pieces
// Ghost piece is drawn in dim color
// Current piece is drawn normally
// Placed pieces are drawn in their respective colors
// Empty spaces are blank
// This function... this funtion is the reason I had to implement console width detection...
// It was a nightmare to get the alignment right, but it works now.
// Also avoids flickering by resetting cursor position instead of clearing screen
// I lost so much sleep over this... I wish I was kidding.
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
                cout << "\033[90m" << "[]" << RESET;
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

// --- Draw next piece centered relative to board ---
// Draws the next piece in a small box to the right of the board
// Centers the box relative to the board
// Uses ANSI colors for pieces
// This was way easier than the main board, somehow...
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

// --- Pause menu ---
// Honestly, this pause menu was not needed for this... but I wanted to try dimming the board out and it was fun to make :)
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

// --- Main ---
// Game loop handling input, timing, and state
// Uses _kbhit and _getch for non-blocking input
// Uses clock() for timing piece falls
// Adjusts fall speed based on level
// Handles piece movement, rotation, hard drop, line clearing, scoring, and game over
// This thing was such a pain to get right, I am pretty sure I lost 3 days of my life to this stupid part.
// But it works now, so whatever.
int main() {
    srand((unsigned)time(nullptr));
    showStartMenu();

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