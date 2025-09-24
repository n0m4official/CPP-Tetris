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

using namespace std;

const int WIDTH = 10;
const int HEIGHT = 20;

string intToString(int n) {
    stringstream ss;
    ss << n;
    return ss.str();
}

// Strip ANSI codes for accurate length
int visibleLength(const string& s) {
    return regex_replace(s, regex("\033\\[[0-9;]*m"), "").size();
}

int getConsoleWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int width = 80;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    return width;
}

void resetCursor() {
    COORD pos = { 0, 0 };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void printCentered(const string& text, int y) {
    int consoleWidth = getConsoleWidth();
    int x = max(0, (consoleWidth - (int)visibleLength(text)) / 2);
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    cout << text;
}

// --- Start menu ---
void showStartMenu() {
    system("cls");
    resetCursor();

    string border = "====================================";
    printCentered("\033[1;36m" + border + "\033[0m", 5);
    printCentered("\033[1;36m           TETRIS v1.0            \033[0m", 6);
    printCentered("\033[1;36m         By n0m4official          \033[0m", 7);
    printCentered("\033[1;36m" + border + "\033[0m", 8);

    printCentered("\033[1;33mControls:\033[0m", 10);
    printCentered("A / D : Move Left / Right", 11);
    printCentered("W     : Rotate", 12);
    printCentered("S     : Soft Drop", 13);
    printCentered("Space : Hard Drop", 14);
    printCentered("Q     : Quit", 15);

    string prompt = "\033[1;32mPress ENTER to start...\033[0m";
    int y = 17;
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

// --- Colors ---
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

// --- Tetrominoes ---
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

struct Piece { 
    vector<vector<int>> shape; 
    int x, y, id; 
};

Piece spawnPiece(int id) { 
    return Piece { 
        TETROMINOES[id], WIDTH / 2 - 2, 0, id 
    }; 
}

int getGhostY(const Board& board, const Piece& current) {
    int y = current.y;
    while (board.isValidPosition(current.shape, current.x, y + 1)) {
        y++;
    }
    return y;
}

int dropDistance(const Board& board, const Piece& piece) {
    int dist = 0;
    while (board.isValidPosition(piece.shape, piece.x, piece.y + dist + 1)) {
        dist++;
    }
    return dist;
}

// --- Draw board centered ---
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

// --- Main ---
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
        bool dirty = true;
        bool running = true;

        while (running) {
            if (_kbhit()) {
                char cmd = _getch();
                if (cmd == 'a' && board.isValidPosition(current.shape, current.x - 1, current.y)) { 
                    current.x--; 
                    dirty = true; 
                }
                if (cmd == 'd' && board.isValidPosition(current.shape, current.x + 1, current.y)) {
                    current.x++; 
                    dirty = true; 
                }
                if (cmd == 'w') {
                    auto r = rotateCW(current.shape);
                    if (board.isValidPosition(r, current.x, current.y)) { 
                        current.shape = r; 
                        dirty = true; 
                    }
                }
                if (cmd == 's') { 
                    if (board.isValidPosition(current.shape, current.x, current.y + 1)) { 
                        current.y++; 
                        dirty = true; 
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
            }

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

            if (dirty) { 
                resetCursor(); 
                printBoard(board, current, score, totalLines, level); 
                printNextPiece(next); 
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
