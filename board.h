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
    vector<vector<sf::Sprite>> baseSprites2D;
    vector<vector<sf::Sprite>> flagSprites2D;
    vector<vector<sf::Sprite>> mineSprites2D;
    vector<vector<sf::Sprite>> numberSprites2D;
    map<string, sf::Texture> boardTextures;     // The map of textures for easy access
    int _rows;
    int _cols;
    int _mines;
    int nonMinesRevealed = 0;
    int minesFlagged = 0;

    // Construct the board
    Board(int rows, int cols, int mines, map<string, sf::Texture>& textures) {
        _rows = rows;
        _cols = cols;
        _mines = mines;
        boardTextures = textures;

        // Initialize Tiles and their respective sprites
        for (int i = 0; i < _rows; i++) {
            vector<Tile*> tileRow;
            vector<sf::Sprite> spriteRow;
            vector<sf::Sprite> flagRow;
            vector<sf::Sprite> mineRow;
            vector<sf::Sprite> numRow;
            for (int j = 0; j < _cols; j++) {
                // Create a new tile
                Tile* tile = new Tile();

                // Add the tile to the row
                tileRow.push_back(tile);    // A single row of tiles in one vector

                // Initializes all tiles to hidden.
                sf::Sprite sprite;
                sprite.setTexture(boardTextures["tile_hidden"]);
                spriteRow.push_back(sprite);

                // Empty flag sprites to begin with.
                sf::Sprite flagSprite;
                flagRow.push_back(flagSprite);

                // Empty mine sprites as well
                sf::Sprite mineSprite;
                mineRow.push_back(mineSprite);

                // Empty number sprites
                sf::Sprite numSprite;
                numRow.push_back(numSprite);
            }
            tiles2D.push_back(tileRow);
            baseSprites2D.push_back(spriteRow);
            flagSprites2D.push_back(flagRow);
            mineSprites2D.push_back(mineRow);
            numberSprites2D.push_back(numRow);
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

            // Check if the tile already has a mine and if not, place it.
            if (!(tiles2D[i][j])->mined) {
                tiles2D[i][j]->mined = true;
                minesToPlace--;
                sf::Sprite mineSprite;
                mineSprite.setTexture(boardTextures["mine"]);
                mineSprites2D[i][j] = mineSprite;
            }
        }

        findNeighbors(_rows, _cols);    // Finds neighbors for each tile

        setNumberSprites(); // Set the number indicators for every tile dependent on the mine locations.
    }

    // Deallocates the tiles
    void eraseTiles() {
        for (int row = 0; row < _rows; row++) {
            tiles2D[row].clear();
        }
    }

    // Determines where the number indicator sprites will be located
    void setNumberSprites() {
        // Have every tile count its adjacent mines
        for (int i = 0; i < tiles2D.size(); i++) {
            for (int j = 0; j < tiles2D[0].size(); j++) {
                int count = tiles2D[i][j]->countAdjacentMines();
                // Set the sprite dependent on what the count is
                sf::Sprite numSprite;
                if (count == 1) {
                    numSprite.setTexture(boardTextures["number_1"]);
                }
                else if (count == 2) {
                    numSprite.setTexture(boardTextures["number_2"]);
                }
                else if (count == 3) {
                    numSprite.setTexture(boardTextures["number_3"]);
                }
                else if (count == 4) {
                    numSprite.setTexture(boardTextures["number_4"]);
                }
                else if (count == 5) {
                    numSprite.setTexture(boardTextures["number_5"]);
                }
                else if (count == 6) {
                    numSprite.setTexture(boardTextures["number_6"]);
                }
                else if (count == 7) {
                    numSprite.setTexture(boardTextures["number_7"]);
                }
                else if (count == 8) {
                    numSprite.setTexture(boardTextures["number_8"]);
                }

                numberSprites2D[i][j] = numSprite;
            }
        }
    }

    // Passes any existing neighboring tiles to each tile
    void findNeighbors(int _rows, int _cols) {

        // Iterate over the 2D vector
        for (int i = 0; i < _rows; i++) {

            for (int j = 0; j < _cols; j++) {
                // For tiles in the top row
                if (i == 0) {
                    // For tiles in the first column
                    if (j == 0) {
                        tiles2D[i][j]->adjacentTiles[4] = tiles2D[i][j + 1];
                        tiles2D[i][j]->adjacentTiles[6] = tiles2D[i + 1][j];
                        tiles2D[i][j]->adjacentTiles[7] = tiles2D[i + 1][j + 1];
                    }
                        // For tiles in the last column
                    else if (j == _cols - 1) {
                        tiles2D[i][j]->adjacentTiles[3] = tiles2D[i][j - 1];
                        tiles2D[i][j]->adjacentTiles[5] = tiles2D[i + 1][j - 1];
                        tiles2D[i][j]->adjacentTiles[6] = tiles2D[i + 1][j];
                    }
                    else {
                        tiles2D[i][j]->adjacentTiles[3] = tiles2D[i][j - 1];
                        tiles2D[i][j]->adjacentTiles[4] = tiles2D[i][j + 1];
                        tiles2D[i][j]->adjacentTiles[5] = tiles2D[i + 1][j - 1];
                        tiles2D[i][j]->adjacentTiles[6] = tiles2D[i + 1][j];
                        tiles2D[i][j]->adjacentTiles[7] = tiles2D[i + 1][j + 1];
                    }
                }
                // For tiles in the bottom row
                else if (i == _rows - 1) {
                    // For tiles in the first column
                    if (j == 0) {
                        tiles2D[i][j]->adjacentTiles[1] = tiles2D[i - 1][j];
                        tiles2D[i][j]->adjacentTiles[2] = tiles2D[i - 1][j + 1];
                        tiles2D[i][j]->adjacentTiles[4] = tiles2D[i][j + 1];
                    }
                        // For tiles in the last column
                    else if (j == _cols - 1) {
                        tiles2D[i][j]->adjacentTiles[0] = tiles2D[i - 1][j - 1];
                        tiles2D[i][j]->adjacentTiles[1] = tiles2D[i - 1][j];
                        tiles2D[i][j]->adjacentTiles[3] = tiles2D[i][j - 1];
                    }
                    else {
                        tiles2D[i][j]->adjacentTiles[0] = tiles2D[i - 1][j - 1];
                        tiles2D[i][j]->adjacentTiles[1] = tiles2D[i - 1][j];
                        tiles2D[i][j]->adjacentTiles[2] = tiles2D[i - 1][j + 1];
                        tiles2D[i][j]->adjacentTiles[3] = tiles2D[i][j - 1];
                        tiles2D[i][j]->adjacentTiles[4] = tiles2D[i][j + 1];
                    }
                }
                // For tiles on the left side
                else if ((j == 0) && (i > 0) && (i < _rows - 1)) {
                    tiles2D[i][j]->adjacentTiles[1] = tiles2D[i - 1][j];
                    tiles2D[i][j]->adjacentTiles[2] = tiles2D[i - 1][j + 1];
                    tiles2D[i][j]->adjacentTiles[4] = tiles2D[i][j + 1];
                    tiles2D[i][j]->adjacentTiles[6] = tiles2D[i + 1][j];
                    tiles2D[i][j]->adjacentTiles[7] = tiles2D[i + 1][j + 1];
                }
                // For tiles on the right side
                else if ((j == _cols - 1) && (i > 0) && (i < _rows - 1)) {
                    tiles2D[i][j]->adjacentTiles[0] = tiles2D[i - 1][j - 1];
                    tiles2D[i][j]->adjacentTiles[1] = tiles2D[i - 1][j];
                    tiles2D[i][j]->adjacentTiles[3] = tiles2D[i][j - 1];
                    tiles2D[i][j]->adjacentTiles[5] = tiles2D[i + 1][j - 1];
                    tiles2D[i][j]->adjacentTiles[6] = tiles2D[i + 1][j];
                }
                // For all other tiles
                else {
                    tiles2D[i][j]->adjacentTiles[0] = tiles2D[i - 1][j - 1];
                    tiles2D[i][j]->adjacentTiles[1] = tiles2D[i - 1][j];
                    tiles2D[i][j]->adjacentTiles[2] = tiles2D[i - 1][j + 1];
                    tiles2D[i][j]->adjacentTiles[3] = tiles2D[i][j - 1];
                    tiles2D[i][j]->adjacentTiles[4] = tiles2D[i][j + 1];
                    tiles2D[i][j]->adjacentTiles[5] = tiles2D[i + 1][j - 1];
                    tiles2D[i][j]->adjacentTiles[6] = tiles2D[i + 1][j];
                    tiles2D[i][j]->adjacentTiles[7] = tiles2D[i + 1][j + 1];
                }
            }
        }
    }
};