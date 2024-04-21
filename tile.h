#pragma once
#include <vector>

using namespace std;

class Tile {
public:
    bool revealed;  // The tile has been revealed or not.
    bool flagged;   // The tile has a flag or not. If flagged, cannot left-click. Right-click removes the flag.
    bool mined;     // The tile has a mine or not
    int adjacentMineCount;  // The number of mines that are adjacent to this tile.

    vector<Tile*> adjacentTiles;    // 2D vector of adjacent tiles

    Tile() {
        revealed = false;
        flagged = false;
        mined = false;
        adjacentMineCount = 0;

        // Initialize the neighbors to nullptr (adjusted later when constructing the board).
        for (int i = 0; i < 8; i++) {
            Tile* tile = nullptr;
            adjacentTiles.push_back(tile);
        }

    }

    // Determines the number of adjacent mines to this tile.
    int countAdjacentMines() {
        for (auto tile : adjacentTiles) {
            if (tile != nullptr && tile->mined) {
                adjacentMineCount++;
            }
        }

        return adjacentMineCount;
    }
};