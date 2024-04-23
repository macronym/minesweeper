#pragma once
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include "board.h"
#include "textures.h"
#include <cmath>
#include <chrono>
#include <fstream>
#include <sstream>

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

    void setWelcomeText(int width, int height) {
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

    // Draws
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
    string name;
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
    bool isNewGame = true;
    bool leaderboardShownAtEndGame = false;
    bool isTopFive = false;
    int newRank = 100;

    // Counter attributes
    int _flagCounter;
    vector<sf::Sprite> mineCounterSprites;     // The counter shows number of "mines" (actually flags placed).
    vector<int> mineCountDigits;        // The digits for the mines
    sf::Sprite negativeSprite;          // Holds the negative sprite, if it exists.

    // Timer attributes
    bool isPaused = true;
    vector<sf::Sprite> minutes;         // The minutes digits of the timer
    vector<sf::Sprite> seconds;         // The seconds digits of the timer
    vector<int> minutesDigits;
    vector<int> secondDigits;
    chrono::duration<double> totalDuration;
    chrono::high_resolution_clock::time_point lastFrameTime;     // Stores the start time of the clock

    // Play/Pause Button
    sf::Sprite pauseButton;

    // Pause Board
    vector<vector<sf::Sprite>> pauseTilesSprites;

    // Leaderboard button
    sf::Sprite leaderButton;

    // Construct the game screen (including the board).
    GameScreen(sf::RenderWindow& window, int width, int height, int numRows, int numCols, int mines, map<string, sf::Texture>& textures) : board(numRows, numCols, mines, textures) {
        _width = width;
        _height = height;
        _numRows = numRows;
        _numCols = numCols;
        _numMines = mines;
        _flagCounter = mines;
        gameTextures = textures;

        // Offset the sprites by designated amount such that the board is displayed in a grid
        setAllBaseSpritesPositions(board.baseSprites2D);

        // Initialize a pause board
        for (int i = 0; i < _numRows; i++) {
            vector<sf::Sprite> spriteRow;
            for (int j = 0; j < _numCols; j++) {
                // Initializes all tiles to hidden.
                sf::Sprite sprite;
                sprite.setTexture(gameTextures["tile_revealed"]);
                spriteRow.push_back(sprite);
            }
            pauseTilesSprites.push_back(spriteRow);
        }

        setGameBackground(_width, _height, sf::Color::White);
        setAllBaseSpritesPositions(pauseTilesSprites);

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

        // Create the timer with default digits 0
        for (int i = 0; i < 2; i++) {
            sf::Sprite minutesDigitSprite;
            minutes.push_back(minutesDigitSprite);
            minutes[i].setTexture(gameTextures["digits"]);
            minutes[i].setPosition((float)((_numCols * 32) - 97 + (i * 21)), (float)((_numRows + 0.5) * 32) + 16);
            minutes[i].setTextureRect(sf::IntRect (0, 0, 21, 32));
        }
        for (int i = 0; i < 2; i++) {
            sf::Sprite secondDigitsSprite;
            seconds.push_back(secondDigitsSprite);
            seconds[i].setTexture(gameTextures["digits"]);
            seconds[i].setPosition((float)((_numCols * 32) - 54 + (i * 21)), (float)((_numRows + 0.5) * 32) + 16);
            seconds[i].setTextureRect(sf::IntRect (0, 0, 21, 32));
        }

        // Create the Pause/Play Button
        pauseButton.setTexture(gameTextures["pause"]);
        pauseButton.setPosition((float)(_numCols * 32) - 240, (float)(_numRows + 0.5) * 32);

        // Create the leaderboard button
        leaderButton.setTexture(gameTextures["leaderboard"]);
        leaderButton.setPosition((_numCols * 32) - 176, 32 * (_numRows + 0.5));
    }

    void setGameBackground(int width, int height, sf::Color color) {
        gameBackground.setSize(sf::Vector2f((float)width, (float)height));
        gameBackground.setFillColor(color);
    }

    // Gets duration of the game in seconds
    void pause() {
        isPaused = true;
    }

    void unpause() {
        isPaused = false;
        lastFrameTime = chrono::high_resolution_clock::now();
    }

    // Gets the username
    void drawToScreen(sf::RenderWindow& window) {

        // The default background
        window.draw(gameBackground);

        // As long as the game is not paused, draw the board
        if (!isPaused || (gameWon || gameLost) || isNewGame) {
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
        }
        // If game is paused, draw a board with all hidden tiles (not the same board)
        else {
            for (int row = 0; row < _numRows; row++) {
                for (int col = 0; col < _numCols; col++) {
                    // Draw pause board
                    window.draw(pauseTilesSprites[row][col]);
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

        if (!isPaused) {
            updateTimer();
        }

        // Draw the timer
        for (const auto& digit : minutes) {
            window.draw(digit);
        }
        for (const auto& digit : seconds) {
            window.draw(digit);
        }

        // Draw the pause button
        window.draw(pauseButton);

        // Draw the leaderboard button
        window.draw(leaderButton);
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
            // If it's a new game and the game is not paused, it must be the first click. This starts the timer.
            if (isNewGame && isPaused) {
                unpause();
                isNewGame = false;
            }
            // Cannot click the board at all if the game is paused.
            else if (!isNewGame && isPaused) {
                return;
            }

            // Reveal a hidden tile
            Tile* tile = board.tiles2D[row][col];

            // Ensure the click was not on a flagged tile
            if (!tile->flagged) {
                // End the game if clicked on a mine.
                if (tile->mined) {
                    gameLost = true;
                    revealAllMines();
                    tile->revealed = true;
                    pause();
                }
                // Reveal tiles as long as they haven't already been revealed.
                if (!tile->revealed) {
                    // Otherwise reveal all non-mine tiles if clicked on a tile with no adjacent mines.
                    if (tile->adjacentMineCount == 0) {
                        floodFillReveal(row, col);
                    }
                    // Otherwise reveal the tile with a mineCount
                    else if (tile->adjacentMineCount > 0) {
                        changeBaseSprite("tile_revealed", row, col);
                        tile->revealed = true;
                        board.nonMinesRevealed++;
                    }
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
            pause();
            storeResult(minutesDigits, secondDigits);       // Stores the final result in leaderboards
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

        // Cannot click if the game is over or paused
        if (gameLost || gameWon || isPaused) {
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

    // Switches between pause/play
    void togglePause() {
        // Cannot toggle when the game is over.
        if (gameLost || gameWon) {
            return;
        }
        else if (isPaused) {
            unpause();
        }
        else {
            pause();
        }

        changePauseSprite();
    }

    // Change the pause button sprite depending on current state.
    void changePauseSprite() {
        if (isPaused) {
            pauseButton.setTexture(gameTextures["play"]);
        }
        else {
            pauseButton.setTexture(gameTextures["pause"]);
        }
    }

    void updateTimer() {
        // Resets the timer to 0 if the game was reset
        if (isNewGame) {
            totalDuration = chrono::duration<double>(0);
        }
        // Updates the timer if active and not paused.
        else {
            auto now = chrono::high_resolution_clock::now();
            totalDuration = totalDuration + now - lastFrameTime;
            lastFrameTime = now;
        }

        // Start with fresh vectors
        minutes.clear();
        seconds.clear();

        // Get the digits for the seconds and minutes.
        int numMin = (int)floor(totalDuration.count()) / 60;    // Gets number of minutes elapsed
        int numSec = (int)floor(totalDuration.count()) % 60;    // Gets number of seconds elapsed

        // Get the digits for the minutes and seconds
        int tempMinuteDigits = numMin;
        int tempSecondsDigits = numSec;
        for (int i = 0; i < 2; i++) {
            int minDigit = tempMinuteDigits % 10;     // Gets the last digit of minutes
            minutesDigits.insert(minutesDigits.begin(), minDigit);     // Inserts into the beginning
            tempMinuteDigits /= 10; // Update temp mine digits

            int secDigit = tempSecondsDigits % 10;     // Gets the last digit of seconds
            secondDigits.insert(secondDigits.begin(), secDigit);     // Inserts into the beginning
            tempSecondsDigits /= 10;
        }

        // Set the digit sprites based on the minutes and seconds.
        for (int i = 0; i < 2; i++) {
            sf::Sprite minutesDigitSprite;
            minutes.push_back(minutesDigitSprite);
            minutes[i].setTexture(gameTextures["digits"]);
            minutes[i].setPosition((float)((_numCols * 32) - 97 + (i * 21)), (float)((_numRows + 0.5) * 32) + 16);
            minutes[i].setTextureRect(sf::IntRect (minutesDigits[i] * 21, 0, 21, 32));
        }
        for (int i = 0; i < 2; i++) {
            sf::Sprite secondDigitsSprite;
            seconds.push_back(secondDigitsSprite);
            seconds[i].setTexture(gameTextures["digits"]);
            seconds[i].setPosition((float)((_numCols * 32) - 54 + (i * 21)), (float)((_numRows + 0.5) * 32) + 16);
            seconds[i].setTextureRect(sf::IntRect (secondDigits[i] * 21, 0, 21, 32));
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

    void setAllBaseSpritesPositions(vector<vector<sf::Sprite>>& tiles) const {
        for (int row = 0; row < _numRows; row++) {
            for (int col = 0; col < _numCols; col++) {
                tiles[row][col].setPosition((float)(32 * col), (float)(32 * row));
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

    void storeResult(vector<int> finalMinutes, vector<int> finalSeconds) {
        // Opens file in read mode
        ifstream infile("files/leaderboard.txt");
        if (!infile) {
            cout << "Error: leaderboard.txt cannot open in read mode." << endl;
        }

        // Store all the previous scores, temporarily because the file will be overwritten
        vector<string> oldRecords;
        string line;
        while (getline(infile, line)) {
            oldRecords.push_back(line);
        }
        infile.close();
        
        // From the temporarily stored scores, get only the score part
        vector<string> oldScores;
        for (const string& record : oldRecords) {
            oldScores.push_back(record.substr(0, 5));
        }

        // Create the new score to be stored
        // Convert time to string form
        ostringstream stream;
        stream << finalMinutes[0];
        stream << finalMinutes[1] << ":";
        stream << finalSeconds[0];
        stream << finalSeconds[1];
        string finalTime = stream.str();
        string newName = name.substr(0, name.size() - 1);       // Ignore the pipe '|' symbol.
        string newRecord = finalTime + ", " + newName;       // The new score that will be stored on the leaderboard


        // Store the current score in the right location
        for (int i = 0; i < oldScores.size(); i++) {
            // If the records match one that already exists, return to avoid duplication.
            if (newRecord == oldRecords[i]) {
                return;
            }

            if ((finalTime <= oldScores[i]) && i <= 5) {
                // Insert into the index
                oldRecords.insert(oldRecords.begin() + i, newRecord);
                isTopFive = true;
                newRank = i;
                break;
            }
        }

        // Write back the top 5 scores
        ofstream outfile("files/leaderboard.txt");
        if (!outfile) {
            cout << "Error: leaderboard.txt cannot open in write mode." << endl;
        }
        for (int i = 0; i < oldRecords.size(); i++) {
          // Only store top 5
          if (i > 4) {
              break;
          }
          outfile << oldRecords[i] << "\n";
        }
        outfile.close();
    }

    void reset() {
        gameLost = false;
        gameWon = false;
        isNewGame = true;
        isPaused = true;
        isTopFive = false;
        newRank = 100;
        leaderboardShownAtEndGame = false;

        // Recreate the tiles
        board.eraseTiles();
        board = Board(_numRows, _numCols, _numMines, gameTextures);
        setAllBaseSpritesPositions(board.baseSprites2D);

        // Reset the face button
        changeFaceSprite();

        // Turns off debug mode if it's on
        if (debugMode) {
            debugMode = false;
        }

        // Resets the mine counter
        _flagCounter = _numMines;
        updateMineCounter();

        // Reset the timer
        updateTimer();

    }
};