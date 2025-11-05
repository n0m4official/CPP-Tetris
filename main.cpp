#define _CRT_SECURE_NO_WARNINGS
#pragma execution_character_set("utf-8")

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
#include "_SecretProtocol.h"


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


// Themes
enum class Theme { Default, Halloween, Christmas, NewYear, Valentines, Birthday, Rememberance };

Theme getSeasonalTheme() {
    time_t t = time(nullptr);
    tm* now = localtime(&t);

    int month = now->tm_mon + 1;
    int day = now->tm_mday;

    if (month == 10 && day >= 20) return Theme::Halloween;
    if (month == 12 && day >= 1) return Theme::Christmas;
    if (month == 1 && day >= 7) return Theme::NewYear;
    if (month == 2 && day >= 10 && day <= 20) return Theme::Valentines;
    if (month == 11 && day == 12)return Theme::Birthday;
	if (month == 11 && day == 11)return Theme::Rememberance;
    return Theme::Default;
}

// --- Constants ---
// Standard Tetris board size of 10x20
const int WIDTH = 10;   ///< Width of the Tetris board
const int HEIGHT = 20;  ///< Height of the Tetris board

// ANSI escape codes for colors (Windows ANSI support)
const string RESET = "\033[0m";
string COLORS[7] = {
	"\033[36m", // Cyan
	"\033[33m", // Yellow
	"\033[35m", // Magenta
	"\033[32m", // Green
	"\033[34m", // Blue
	"\033[31m", // Red
	"\033[91m"  // Bright Red
};

auto colorFor = [&](int id)->const char* {
    if (Secret::UseRetroMonochrome()) return "\033[1;37m";
    if (Secret::UseRedTint()) return "\033[31m";
    if (Secret::UseGrayNight()) return "\033[90m";
    if (Secret::UseRainbowPieces()) { static const char* R[] = { "\033[31m","\033[33m","\033[32m","\033[36m","\033[34m","\033[35m","\033[91m" }; return R[(id + rand()) % 7]; }
    if (Secret::UseColorChaos()) { static const char* R[] = { "\033[31m","\033[33m","\033[32m","\033[36m","\033[34m","\033[35m","\033[91m" }; return R[rand() % 7]; }
    return nullptr;
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

    Theme theme = getSeasonalTheme();

    string bannerColor = "\033[1;36m"; // default cyan
    string title = "TETRIS v2.0.0";
    string subtitle = "By n0m4official";

    switch (theme) {
    case Theme::Halloween:
        bannerColor = "\033[38;5;208m"; // orange
        title = "🎃 TETRIS: Spooky Blocks 🎃";
        break;
    case Theme::Christmas:
        bannerColor = "\033[1;32m"; // green
        title = "🎄 TETRIS: Holiday Edition 🎄";
        subtitle = "Ho-Ho-Hold those pieces!";
        break;
    case Theme::NewYear:
        bannerColor = "\033[1;33m"; // gold
        title = "✨ TETRIS: New Year Special ✨";
        break;
    case Theme::Valentines:
        bannerColor = "\033[1;35m"; // magenta
        title = "💖 TETRIS: Love Blocks 💖";
        break;
    case Theme::Birthday:
        bannerColor = "\033[1;33m"; // gold
        subtitle = "Happy birthday to the dev!";
        break;
    case Theme::Rememberance:
		bannerColor = "\033[1;30m"; // gray
		title = "🕯️ TETRIS: In Remembrance 🕯️";
		subtitle = "Lest we forget.";
		break;
    default:
        break;
    }

    Secret::ApplySeasonalBanner(bannerColor, title, subtitle);

    printCentered(bannerColor + "====================================" + "\033[0m", 5);
    printCentered(bannerColor + title + "\033[0m", 6);
    printCentered(bannerColor + (subtitle.empty() ? std::string("By n0m4official") : subtitle) + "\033[0m", 7);
    printCentered(bannerColor + "====================================" + "\033[0m", 8);

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
            Secret::OnTitleKey(c);
            if (c == '\r') break; // start game
            if (c == 'c' || c == 'C') {
                customizeColors();
                system("cls");
                showStartMenu(); // restart menu after customizing
                return;
            }
            if (c == 'q' || c == 'Q') exit(0);
        }
        Secret::StartMenuOverpaintTick();
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

// If you lose, at least you can try again.
// Returns true if player wants to restart, false to quit
// Also handles highscore entry and display
// Because everyone loves a good leaderboard.
// I spent way too much time on this part, why am I like this...
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

    double multiplier = 1.0;
    bool hideScoreText = false;
    Secret::OnPlayerNameEntered(playerName, score, multiplier, hideScoreText);

    score = int(score * multiplier);

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
    // If you change this, Tetris gods get angy.
};

// Spawn a new piece at the top center
// Like a fresh start, but with more anxiety.
// id: 0-6 for the 7 tetrominoes
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
// Returns how many rows the piece can drop
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
Attempt to rotate piece with SRS wall kicks
*/
// Returns true if rotation succeeded, false otherwise
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
// Enhanced printBoard with secret visual modes integrated
// Because Tetris should be a visual spectacle.
// Also includes score, lines, and level display
// along with various overlays based on secret settings
// and seasonal themes.
// Note: This function assumes console supports ANSI escape codes.
// If not, colors will not display correctly.
// Also includes centering logic based on console width.
void printBoard(const Board& board, const Piece& current, int score, int lines, int level) {
    int consoleWidth = getConsoleWidth();
    int boardWidth = WIDTH * 2 + 2;
    int offsetX = max(0, (consoleWidth - boardWidth) / 2);

    COORD pos = { 0, 0 };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

    // Hide score text for 404 easter egg
    if (!Secret::HideScoreText()) {
        cout << string(offsetX, ' ') << "Score: " << score << "  Lines: " << lines << "  Level: " << level << "\n\n";
    }
    else {
        cout << string(offsetX, ' ') << "\033[90mScore hidden\033[0m\n\n";
    }

    cout << string(offsetX, ' ') << "+" << string(WIDTH * 2, '-') << "+\n";

    int ghostY = getGhostY(board, current);
    bool alwaysGhost = Secret::ForceGhostAlwaysVisible();

    for (int y = 0; y < HEIGHT; ++y) {
        cout << string(offsetX, ' ') << "|";
        for (int x = 0; x < WIDTH; ++x) {
            bool occupied = board.grid[y][x] != 0;
            int pieceId = occupied ? board.grid[y][x] - 1 : -1;
            int sx = x - current.x, sy = y - current.y;

            const char* color = colorFor(occupied ? pieceId : current.id);
            if (!color) color = COLORS[occupied ? pieceId : current.id].c_str();

            if (sx >= 0 && sx < (int)current.shape[0].size() && sy >= 0 && sy < (int)current.shape.size() && current.shape[sy][sx]) {
                cout << color << "[]" << RESET;
            }
            else if ((alwaysGhost || (!occupied && y >= ghostY && y < ghostY + (int)current.shape.size())) &&
                sx >= 0 && sx < (int)current.shape[0].size() && current.shape[y - ghostY][sx]) {
                cout << "\033[90m[]\033[0m"; // ghost piece
            }
            else if (occupied) {
                cout << color << "[]" << RESET;
            }
            else {
                cout << "  ";
            }
        }
        cout << "|\n";
    }

    cout << string(offsetX, ' ') << "+" << string(WIDTH * 2, '-') << "+\n";

    // --- Extra overlays ---
    // Low battery flicker effect
    if (Secret::ShouldFlickerLowBattery() && rand() % 25 == 0) {
        cout << "\033[1;33mWARNING: LOW BATTERY\033[0m\n";
    }

    // Hydration reminder after long session
    if (Secret::ShouldShowHydration() && rand() % 50 == 0) {
        cout << "\033[1;34m💧 Remember to stay hydrated!\033[0m\n";
    }

    // CRT overlay effect (faint scanline simulation)
    if (Secret::UseCRTOverlay()) {
        for (int y = 0; y < 2; ++y) {
            cout << string(offsetX, ' ') << "\033[90m" << string(WIDTH * 2, '=') << "\033[0m\n";
        }
    }

    // Matrix rain or fireworks visual tick (simple ASCII bursts)
    if (Secret::UseMatrixRain() && rand() % 10 == 0) {
		COORD pos2 = { (SHORT)rand() % consoleWidth, (SHORT)(rand() % 20 + 3) }; // avoid overwriting score line
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos2); // set random position
		cout << "\033[32m" << (char)(rand() % 93 + 33) << "\033[0m"; // random green char
    }
}

// Draw next piece preview
const int QUEUE_SIZE = 3; // number of next pieces to show
std::queue<Piece> nextQueue; // global queue

// Draw next pieces from the queue
// This thing took me WAAAAAAAAAAY too long to get working.
// If you break it, you fix it.
void printNextQueue(const std::queue<Piece>& q) { // pass by const reference
    std::queue<Piece> copy = q; // local copy

	int consoleWidth = getConsoleWidth(); // get console width
	int boxWidth = 8; // 4 blocks * 2
	int offsetX = max(0, (consoleWidth - WIDTH * 2 - boxWidth) / 2 + WIDTH + 20); // to the right of board
	int verticalOffset = 3; // start a bit lower

	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); // get console handle

    for (int i = 0; i < QUEUE_SIZE && !copy.empty(); i++) {
		Piece p = copy.front(); // get next piece
		copy.pop(); // remove it from copy

        for (int y = 0; y < 6; y++) {
			COORD pos = { (SHORT)offsetX, (SHORT)(verticalOffset + y + i * 6) }; // position for each piece
			SetConsoleCursorPosition(hOut, pos); // set cursor

			if (y == 0 || y == 5) {
				cout << "+" << string(8, '-') << "+"; // top/bottom border
            }
            else {
				cout << "|"; // left border
				for (int x = 0; x < 4; x++) { // 4 blocks wide
					if (p.shape[y - 1][x]) { // if block exists
						cout << COLORS[p.id] << "[]" << RESET; // colored block
                    }
                    else {
						cout << "  "; // empty space
                    }
                }
				cout << "|"; // right border
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
	int consoleWidth = getConsoleWidth(); // get console width
	int boardWidth = WIDTH * 2 + 2; // board width with borders
	int offsetX = max(0, (consoleWidth - boardWidth) / 2); // center board

	resetCursor(); // reset cursor to top-left
	cout << string(offsetX, ' ') << "Score: " << score << "  Lines: " << lines << "  Level: " << level << "\n\n"; // score line
	cout << string(offsetX, ' ') << "+" << string(WIDTH * 2, '-') << "+\n"; // top border

	int ghostY = getGhostY(board, current); // calculate ghost Y position
	for (int y = 0; y < HEIGHT; ++y) { // for each row
		cout << string(offsetX, ' ') << "|"; // left border
		for (int x = 0; x < WIDTH; ++x) { // for each column
			bool occupied = board.grid[y][x] != 0; // check if occupied
			int pieceId = occupied ? board.grid[y][x] - 1 : -1; // get piece ID
			int sx = x - current.x, sy = y - current.y; // relative to current piece

			if (sx >= 0 && sx < (int)current.shape[0].size() &&
				sy >= 0 && sy < (int)current.shape.size() &&
				current.shape[sy][sx]) { // current piece position check
                cout << "\033[90m[]" << RESET; // dim current piece
            }
			else if (!occupied && y >= ghostY && y < ghostY + (int)current.shape.size() &&
				sx >= 0 && sx < (int)current.shape[0].size() && current.shape[y - ghostY][sx]) { // ghost piece check
                cout << "\033[90m[]" << RESET; // dim ghost
            }
			else if (occupied) { // occupied cell
                cout << "\033[90m[]" << RESET; // dim placed block
            }
			else { // empty cell
                cout << "  ";
            }
        }
		cout << "|\n"; // right border
    }
	cout << string(offsetX, ' ') << "+" << string(WIDTH * 2, '-') << "+\n"; // bottom border

    // Overlay text
	string pauseText = "\033[1;36m=== PAUSED ===\033[0m"; // cyan
	string resumeText = "\033[1;33mPress P to Resume or Q to Quit\033[0m"; // yellow

	int y = HEIGHT / 2 - 1; // vertical center
	printCentered(pauseText, y); // print pause text
	printCentered(resumeText, y + 2); // print resume/quit text
}

// I have tried several times to stop this from being redrawn after every update, still cannot figure it out as of 03-11-2025.
// If you solve this, I will be forever grateful.
// Draw held piece
void printHoldPiece(const Piece& hold) {
	int consoleWidth = getConsoleWidth(); // get console width
    int boxWidth = 8; // 4 blocks * 2
	int offsetX = max(0, (consoleWidth - WIDTH * 2 - boxWidth) / 2 - boxWidth - 4); // to the left of board

	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); // get console handle
	int verticalOffset = 3; // start a bit lower

	for (int y = 0; y < 6; ++y) { // 6 rows for box
		COORD pos = { (SHORT)offsetX, (SHORT)(y + verticalOffset) }; // position for each row
		SetConsoleCursorPosition(hOut, pos); // set cursor

		if (y == 0 || y == 5) { // top/bottom border
			cout << "+" << string(8, '-') << "+"; // 8 = 4 blocks * 2
        }
		else { // middle rows
			cout << "|"; // left border
			for (int x = 0; x < 4; ++x) { // 4 blocks wide
				if (hold.id != -1 && hold.shape[y - 1][x]) { // if block exists
					cout << COLORS[hold.id] << "[]" << RESET; // colored block
                }
				else { // empty space
					cout << "  "; // two spaces
                }
            }
			cout << "|"; // right border
        }
    }
}

// --- Main Game Loop ---
// If you make it through this loop, you deserve a break.
int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    srand((unsigned)time(nullptr));
    loadColors();
    Secret::Init();
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

        Secret::OnSessionStart();

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
                        Secret::OnHardDrop();

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
                    Secret::OnSessionTick(score, level, totalLines, paused);
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
                        Secret::OnLockAndClear(cleared);
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