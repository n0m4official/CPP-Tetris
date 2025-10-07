#pragma once
#include <vector>
using namespace std;

class Board {
public:
    vector<vector<int>> grid; ///< 2D board grid
    Board();
    bool isValidPosition(const vector<vector<int>>& shape, int x, int y) const;
    void placePiece(const vector<vector<int>>& shape, int x, int y, int id);
    int clearLines();
    int scoreForLines(int lines, int level);
};
