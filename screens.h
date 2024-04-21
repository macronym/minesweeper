#pragma once
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include "board.h"
#include "textures.h"
#include <cmath>

using namespace std;

// Centers the text (Useful for Welcome Screen)
void setText(sf::Text &text, float x, float y){
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f, textRect.top + textRect.height/2.0f);
    text.setPosition(sf::Vector2f(x, y));
}

class WelcomeScreen {
public:
    // States to save
    bool active = true;    // If true, show the welcome screen
    string name = "|";    // Stores the name inputted by the user.
    int _width;
    int _height;
    sf::Font font;  // The chosen font for the text
    sf::RectangleShape welcomeBackground;
    sf::Text welcomeText;
    sf::Text namePrompt;
    sf::Text userNameField;

    // Constructor
    WelcomeScreen(sf::RenderWindow& window, int width, int height) {
        _width = width;
        _height = height;
        createWelcomeScreen();
    }

    // Declares what the welcome screen should look like
    int createWelcomeScreen() {
        // Set background
        setWelcomeBackground(_width, _height, sf::Color::Blue);

        // Get the font from files
        string filename = "files/font.ttf";
        if (!font.loadFromFile(filename))
        {
            cout << "Font file failed to load." << endl;
            return 2;
        }

        // Set welcome text
        setWelcomeText(_width, _height);

        // Set name prompt text
        setNamePromptText(_width, _height);

        // Set the username field
        setUserNameField(_width, _height);

        // No known errors.
        return 0;
    }

    void setWelcomeBackground(int width, int height, sf::Color color) {
        // Set background size and color
        welcomeBackground.setSize(sf::Vector2f((float)width, (float)height));
        welcomeBackground.setFillColor(color);
    }

    int setWelcomeText(int width, int height) {
        // Text that greets the user
        welcomeText.setFont(font);
        welcomeText.setString("WELCOME TO MINESWEEPER!");
        welcomeText.setCharacterSize(24);
        welcomeText.setFillColor(sf::Color::White);
        welcomeText.setStyle(sf::Text::Bold | sf::Text::Underlined);
        setText(welcomeText, (float)width / 2, (float)(height / 2 - 150)); // NOLINT(*-integer-division)
    }

    void setNamePromptText(int width, int height) {
        // Text that prompts for a username input
        namePrompt.setFont(font);
        namePrompt.setString("Enter your name:");
        namePrompt.setCharacterSize(20);
        namePrompt.setFillColor(sf::Color::White);
        namePrompt.setStyle(sf::Text::Bold);
        setText(namePrompt, (float)width / 2, (float)(height / 2 - 75)); // NOLINT(*-integer-division)
    }

    void setUserNameField(int width, int height) {
        // Text that the user can input, saved for later.
        userNameField.setFont(font);
        userNameField.setString(name);
        userNameField.setCharacterSize(18);
        userNameField.setFillColor(sf::Color::Yellow);
        userNameField.setStyle(sf::Text::Bold);
        setText(userNameField, (float)width / 2, (float)(height / 2 - 45)); // NOLINT(*-integer-division)
    }

    // Adds a character to the username
    void addCharacter(char& character) {
        // Pops the pipe before adding the character + pipe.
        name.pop_back();

        // Add with necessary case
        if (!name.empty()) {
            name += tolower(character);
        }
        else if (name.empty()) {
            name += toupper(character);
        }
        name += "|";

        // Testing
        cout << name << endl;

        // Sets the new string.
        userNameField.setString(name);
        setText(userNameField, (float)_width / 2, (float)(_height / 2 - 45)); // NOLINT(*-integer-division)
    }

    // Deletes the last character from the username.
    void popCharacter() {
        // Only possible if name has at least one letter.
        if (name.length() > 1) {
            name.pop_back();
            name.pop_back();
            name += "|";
        }

        // Sets the new string.
        userNameField.setString(name);
        setText(userNameField, (float)_width / 2, (float)(_height / 2 - 45)); // NOLINT(*-integer-division)
    }

    // Gets the username
    void drawToScreen(sf::RenderWindow& window) {
        window.draw(welcomeBackground);
        window.draw(welcomeText);
        window.draw(namePrompt);
        window.draw(userNameField);
    }

    // Declares the welcome screen active
    void activate() {
        active = true;
    }

    // Declares the welcome screen inactive
    void deactivate() {
        active = false;
    }
};

class GameScreen {
public:
    bool active = false;
    int _width;
    int _height;
    int _numRows;
    int _numCols;
    int _numMines;
    sf::RectangleShape gameBackground;
    Board board;
    sf::Sprite happyFaceButton;
    sf::Sprite debugButton;
    map<string, sf::Texture> gameTextures;
    bool gameLost = false;
    bool gameWon = false;
    bool debugMode = false;

    int _flagCounter;
    vector<sf::Sprite> mineCounterSprites;     // The counter shows number of "mines" (actually flags placed).
    vector<int> mineCountDigits;        // The digits for the mines
    sf::Sprite negativeSprite;          // Holds the negative sprite, if it exists.

    // Construct the game screen (including the board).
    GameScreen(sf::RenderWindow& window, int width, int height, int numRows, int numCols, int mines, map<string, sf::Texture>& textures) : board(numRows, numCols, mines, textures) {
        _width = width;
        _height = height;
        _numRows = numRows;
        _numCols = numCols;
        _numMines = mines;
        _flagCounter = mines;
        gameTextures = textures;
        createGameScreen();

        // Offset the sprites by designated amount such that the board is displayed in a grid
        setAllBaseSpritesPositions();

        // Create the Happy Face Button
        happyFaceButton.setTexture(gameTextures["face_happy"]);
        happyFaceButton.setPosition((float)((_numCols/2.0) * 32) - 32, (float)(32 * (_numRows + 0.5)));

        // Create the debug button
        debugButton.setTexture(gameTextures["debug"]);
        debugButton.setPosition((float)(_numCols * 32) - 304, (float)(_numRows + 0.5) * 32);

        // Create the mine counter
        // Set the negative sprite
        negativeSprite.setTexture(gameTextures["digits"]);
        negativeSprite.setTextureRect(sf::IntRect (10 * 21, 0, 21, 32));
        negativeSprite.setPosition((float)(12), (float)((_numRows + 0.5) * 32) + 16);

        // Get the digits for the mine count.
        int tempMineDigits = mines;
        for (int i = 0; i < 3; i++) {
            int digit = tempMineDigits % 10;     // Gets the last digit
            mineCountDigits.insert(mineCountDigits.begin(), digit);     // Inserts into the beginning
            tempMineDigits /= 10; // Update temp mine digits
        }

        // Set the digit sprites based on the mineCount.
        int startingPixel = 33;
        for (int i = 0; i < 3; i++) {
            sf::Sprite digitsSprite;
            mineCounterSprites.push_back(digitsSprite);
            mineCounterSprites[i].setTexture(gameTextures["digits"]);
            mineCounterSprites[i].setTextureRect(sf::IntRect (mineCountDigits[i] * 21, 0, 21, 32));
            mineCounterSprites[i].setPosition((float)startingPixel, (float)((_numRows + 0.5) * 32) + 16);
            startingPixel += 21;    // Each subsequent digit is 21 pixels further to the right.
        }

    }

    void createGameScreen() {
        // Testing
        setGameBackground(_width, _height, sf::Color::White);
    }

    void setGameBackground(int width, int height, sf::Color color) {
        // Testing
        gameBackground.setSize(sf::Vector2f((float)width, (float)height));
        gameBackground.setFillColor(color);
    }

    // Gets the username
    void drawToScreen(sf::RenderWindow& window) {
        window.draw(gameBackground);

        // Draw all the sprites
        for (int row = 0; row < _numRows; row++) {
            for (int col = 0; col < _numCols; col++) {
                // Draw base sprites
                window.draw(board.baseSprites2D[row][col]);
                // Draw any necessary flags
                if (board.tiles2D[row][col]->flagged) {
                    window.draw(board.flagSprites2D[row][col]);
                }
                // Draw the numbers if they exist
                if (board.tiles2D[row][col]->revealed && !board.tiles2D[row][col]->mined && board.tiles2D[row][col]->adjacentMineCount > 0) {
                    setNumberSpritesPosition(row, col);
                    window.draw(board.numberSprites2D[row][col]);
                }
                // If the game is over, draw all mines and change the face button
                if (gameLost && board.tiles2D[row][col]->mined) {
                    setMinesSpritesPosition(row, col);
                    window.draw(board.mineSprites2D[row][col]);
                    changeFaceSprite();
                }
                // If debug mode is on draw the mine sprites.
                else if (debugMode && !gameWon && board.tiles2D[row][col]->mined){
                    setMinesSpritesPosition(row, col);
                    window.draw(board.mineSprites2D[row][col]);
                }
            }
        }

        // Draw the happy face
        window.draw(happyFaceButton);

        // Draw the debug button
        window.draw(debugButton);

        // Draw the mine counter
        for (const auto& digit : mineCounterSprites) {
            window.draw(digit);
            if (_flagCounter < 0) {
                window.draw(negativeSprite);
            }
        }

    }

    void leftClickAction(int mouseX, int mouseY) {
        // Do an action depending on the location of the click

        // Cannot click if the game is over
        if (gameLost || gameWon) {
            return;
        }

        // Get index of the tiles
        int row = mouseY / 32;
        int col = mouseX / 32;

        // Left clicks within the tiles
        if (mouseY < _height - 100) {
            // Reveal a hidden tile
            Tile* tile = board.tiles2D[row][col];

            // Ensure the click was not on a flagged tile
            if (!tile->flagged) {
                // End the game if clicked on a mine.
                if (tile->mined) {
                    gameLost = true;
                    revealAllMines();
                    tile->revealed = true;
                }
                // Otherwise reveal all non-mine tiles if clicked on a tile with no adjacent mines.
                else if (tile->adjacentMineCount == 0) {
                    floodFillReveal(row, col);
                }
                // Otherwise reveal the tile with a mineCount
                else if (tile->adjacentMineCount > 0) {
                    changeBaseSprite("tile_revealed", row, col);
                    tile->revealed = true;
                    board.nonMinesRevealed++;
                }
            }

            checkForEndGame();

        }
    }

    void checkForEndGame() {
        // Check if all non-mine tiles have been revealed and all mine tiles have been flagged
        // and if so, the game is won.
        if ((board.nonMinesRevealed == (_numRows * _numCols) - board._mines) && (board._mines == board.minesFlagged)){
            gameWon = true;
            changeFaceSprite();
        }
    }

    // Change every tile with a mine on it to be revealed. Used at end-game if the user lost.
    void revealAllMines() {
        for (int i = 0; i < board.tiles2D.size(); i++) {
            for (int j = 0; j < board.tiles2D[0].size(); j++) {

                // If the game is lost, reveal that tile if it has a mine on it.
                if (gameLost && board.tiles2D[i][j] != nullptr && board.tiles2D[i][j]->mined) {
                    changeBaseSprite("tile_revealed", i, j);
                }
                else if (debugMode && board.tiles2D[i][j] != nullptr && board.tiles2D[i][j]->mined) {

                }
            }
        }
    }

    void toggleDebugMode() {
        if (debugMode) {
            debugMode = false;
        }
        else {
            debugMode = true;
        }
    }


    // Recursively reveals all empty tiles when clicking on one.
    void floodFillReveal(int row, int col) {
        // Ensure the row or column is within the bounds of the board
        int maxRow = board.tiles2D.size() - 1;
        int maxCol = board.tiles2D[0].size() - 1;
        if (row < 0 || row > maxRow || col < 0 || col > maxCol) {
            return;
        }
        // Stop if the tile is already revealed
        Tile* tile = board.tiles2D[row][col];
        if (tile->mined || tile->revealed) return;

        // Check the tile is adjacent to an empty tile.
        bool hasEmptyAdjacent = false;
        for (auto & adjacentTile : tile->adjacentTiles) {
            if (adjacentTile != nullptr && adjacentTile->adjacentMineCount == 0) {
                hasEmptyAdjacent = true;
                break;
            }
        }

        if (!tile->flagged && !tile->revealed && (hasEmptyAdjacent || tile->adjacentMineCount == 0)) {
            cout << "Row: " << row << " Col: " << col << endl;
            changeBaseSprite("tile_revealed", row, col);
            tile->revealed = true;
            board.nonMinesRevealed++;
        }
        // Then return if the tile had a number
        if (tile->flagged || tile->adjacentMineCount > 0) {
            return;
        }

        floodFillReveal(row + 1, col);  // Move down
        floodFillReveal(row - 1, col);  // Move up
        floodFillReveal(row, col + 1);  // Move right
        floodFillReveal(row, col - 1);  // Move left
        floodFillReveal(row + 1, col + 1); // Move bottom-right
        floodFillReveal(row - 1, col + 1);  // Move top-right
        floodFillReveal(row - 1, col - 1); // Move top-left
        floodFillReveal(row + 1, col - 1); // Move bottom-left
    }

    // Does an action depending on where right-clicking
    void rightClickAction(int mouseX, int mouseY) {

        // Cannot click if the game is over
        if (gameLost || gameWon) {
            return;
        }

        // Get index of the tiles
        int row = mouseY / 32;
        int col = mouseX / 32;

        // Right clicks within the tiles
        if (mouseY < _height - 100) {
            // Flag or unflag a tile
            Tile* tile = board.tiles2D[row][col];
            if (!tile->revealed) {
                sf::Sprite sprite;
                if (!tile->flagged) {
                    tile->flagged = true;
                    _flagCounter--;
                    updateMineCounter();
                    sprite.setTexture(gameTextures["flag"]);
                    if (tile->mined) {
                        board.minesFlagged++;
                    }
                }
                else if (tile->flagged) {
                    tile->flagged = false;
                    _flagCounter++;
                    updateMineCounter();
                    if (tile->mined) {
                        board.minesFlagged--;
                    }
                }
                // Update sprite
                board.flagSprites2D[row][col] = sprite;
                setFlagSpritePosition(row, col);
            }
        }

        checkForEndGame();
    }

    // Changes the tile that was clicked on to the given texture, if possible.
    void changeBaseSprite(string textureName, int row, int col) {
        // Stores the new sprite at that index in the board
        sf::Sprite sprite;
        sprite.setTexture(gameTextures[textureName]);
        board.baseSprites2D[row][col] = sprite;
        setBaseSpritePosition(row, col);
    }

    // Changes the face depending on win/loss
    void changeFaceSprite() {
        if (gameLost) {
            happyFaceButton.setTexture(gameTextures["face_lose"]);
        }
        else if (gameWon) {
            happyFaceButton.setTexture(gameTextures["face_win"]);
        }
        else {
            happyFaceButton.setTexture(gameTextures["face_happy"]);
        }
    }

    // Update mineCounterSprites;
    void updateMineCounter() {

        // Start with fresh vectors
        mineCounterSprites.clear();
        mineCountDigits.clear();

        // Get the digits for the mine count.
        int tempMineDigits = _flagCounter;
        for (int i = 0; i < 3; i++) {
            int digit = tempMineDigits % 10;     // Gets the last digit
            mineCountDigits.insert(mineCountDigits.begin(), digit);     // Inserts into the beginning
            tempMineDigits /= 10; // Update temp mine digits
        }

        // Set the digit sprites based on the mineCount.
        // Store the rest of the digits
        int startingPixel = 33;
        for (int i = 0; i < 3; i++) {
            sf::Sprite digitsSprite;
            mineCounterSprites.push_back(digitsSprite);
            mineCounterSprites[i].setTexture(gameTextures["digits"]);
            mineCounterSprites[i].setTextureRect(sf::IntRect (abs(mineCountDigits[i]) * 21, 0, 21, 32));
            mineCounterSprites[i].setPosition((float)startingPixel, (float)((_numRows + 0.5) * 32) + 16);
            startingPixel += 21;    // Each subsequent digit is 21 pixels further to the right.
        }

    }

    void setAllBaseSpritesPositions() {
        for (int row = 0; row < _numRows; row++) {
            for (int col = 0; col < _numCols; col++) {
                board.baseSprites2D[row][col].setPosition((float)(32 * col), (float)(32 * row));
            }
        }
    }

    void setBaseSpritePosition(int row, int col) {
        board.baseSprites2D[row][col].setPosition((float)(32 * col), (float)(32 * row));
    }

    void setFlagSpritePosition(int row, int col) {
        board.flagSprites2D[row][col].setPosition((float)(32 * col), (float)(32 * row));
    }

    void setMinesSpritesPosition(int row, int col) {
        board.mineSprites2D[row][col].setPosition((float)(32 * col), (float)(32 * row));
    }

    void setNumberSpritesPosition(int row, int col) {
        board.numberSprites2D[row][col].setPosition((float)(32 * col), (float)(32 * row));
    }

    void activate() {
        active = true;
    }

    void deactivate() {
        active = false;
    }

    void reset() {
        board.eraseTiles();
        gameLost = false;
        gameWon = false;
        board = Board(_numRows, _numCols, _numMines, gameTextures);
        setAllBaseSpritesPositions();
        changeFaceSprite();
        if (debugMode) {
            toggleDebugMode();
        }
    }
};