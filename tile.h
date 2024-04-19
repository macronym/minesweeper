#pragma once
#include <vector>

using namespace std;

class Tile {
public:
    bool revealed = false;  // The tile has been revealed or not.
    bool flagged = false;   // The tile has a flag or not. If flagged, cannot left-click. Right-click removes the flag.
    bool mined = false;     // The tile has a mine or not

    // List of neighboring tiles (up to 8 possible)
    vector<Tile*> adjacentTiles;
};