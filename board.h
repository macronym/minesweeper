#pragma once
#include "tile.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

using namespace std;

class Board {
public:
    // 2D vector of tiles and sprites for filling the board
    vector<vector<Tile*>> tiles2D;
    vector<vector<sf::Sprite>> sprites2D;
    map<string, sf::Texture> boardTextures;     // The map of textures for easy access
    int _width;
    int _height;
    int _rows;
    int _cols;
    int _mines;

    // Construct the board
    Board(int width, int height, int rows, int cols, int mines, map<string, sf::Texture>& textures) {
        _width = width;
        _height = height;
        _rows = rows;
        _cols = cols;
        _mines = mines;
        boardTextures = textures;

        // Initialize Tiles and their respective sprites
        for (int i = 0; i < _rows; i++) {
            vector<Tile*> tileRow;
            vector<sf::Sprite> spriteRow;
            for (int j = 0; j < _cols; j++) {
                // Create a new tile
                Tile* tile = new Tile();

                // Add the tile to the row
                tileRow.push_back(tile);    // A single row of tiles in one vector

                // Initializes all tiles to hidden.
                sf::Sprite sprite;
                sprite.setTexture(boardTextures["tile_hidden"]);
                spriteRow.push_back(sprite);
            }
            tiles2D.push_back(tileRow);
            sprites2D.push_back(spriteRow);
        }

        // Randomly place mines until the mines quota is reached.
        // ***From Functor Slides***
        mt19937 mt(time(0));
        uniform_int_distribution<int> distRow(0, _rows - 1);
        uniform_int_distribution<int> distCol(0, _cols - 1);

        int minesToPlace = _mines;
        while (minesToPlace > 0) {
            // Generate random numbers
            int i = distRow(mt);
            int j = distCol(mt);

            cout << "j: " << j << endl;
            cout << "i: " << i << endl;
            cout << "rows: " << _rows << endl;
            cout << "cols: " << _cols << endl;
            cout << "size of textures: " << boardTextures.size() << endl;
            cout << "size of sprites: " << sprites2D.size() << endl;
            cout << "size of tiles: " << tiles2D.size() << endl;
            cout << "size of tiles rows " << tiles2D[0].size() << endl;

            // Check if the tile already has a mine and if not, place it.
            if (!(tiles2D[i][j])->mined) {
                tiles2D[i][j]->mined = true;
                minesToPlace--;
                cout << "Mines left: " << minesToPlace << endl;
            }
        }

        // Declare tile neighbors
    };
};