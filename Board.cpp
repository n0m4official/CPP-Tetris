#include "Board.h"
#include <algorithm>

// --- Board & logic ---
Board::Board() : grid(20, vector<int>(10, 0)) {}    ///< Board size (10x20 - standard Tetris size)

// Checks if the position is reachable and able to fit the current piece
bool Board::isValidPosition(const vector<vector<int>>& shape, int x, int y) const {
    int h = (int)shape.size();
    int w = (int)shape[0].size();
    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j)
            if (shape[i][j]) {
                int nx = x + j, ny = y + i;
                if (nx < 0 || nx >= 10 || ny < 0 || ny >= 20) return false;
                if (grid[ny][nx] != 0) return false;
            }
    return true;
}

// Piece placing logic
void Board::placePiece(const vector<vector<int>>& shape, int x, int y, int id) {
    for (int i = 0; i < (int)shape.size(); ++i)
        for (int j = 0; j < (int)shape[0].size(); ++j)
            if (shape[i][j]) grid[y + i][x + j] = id + 1;
}

// Handles line clearing
int Board::clearLines() {
    int cleared = 0;
    for (int row = 19; row >= 0; --row) {
        if (all_of(grid[row].begin(), grid[row].end(), [](int c) {return c != 0; })) {
            grid.erase(grid.begin() + row);
            grid.insert(grid.begin(), vector<int>(10, 0));
            ++cleared;
            ++row; // recheck same row index
        }
    }
    return cleared;
}

// Scores for number of cleared lines
int Board::scoreForLines(int lines, int level) {
    switch (lines) {
    case 1: return 40 * (level + 1);
    case 2: return 100 * (level + 1);
    case 3: return 300 * (level + 1);
    case 4: return 1200 * (level + 1);
    default: return 0;
    }
}
