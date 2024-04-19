#pragma once
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include "board.h"

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
    sf::RectangleShape gameBackground;
    Board board;

    // Construct the game screen (including the board).
    GameScreen(sf::RenderWindow& window, int width, int height, int numRows, int numCols, int mines, map<string, sf::Texture>& textures) : board(width, height, numRows, numCols, mines, textures) {
        _width = width;
        _height = height;
        _numRows = numRows;
        _numCols = numCols;
        createGameScreen();

        // Offset the sprites by designated amount such that the board is displayed in a grid
        for (int row = 0; row < _numRows; row++) {
            for (int col = 0; col < _numCols; col++) {
                board.sprites2D[row][col].setPosition((float)(32*col), (float)(32*row));
            }
        }

    }

    //
    void createGameScreen() {
        // Testing
        setGameBackground(_width, _height, sf::Color::Red);


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
                window.draw(board.sprites2D[row][col]);
            }
        }

    }

    void activate() {
        active = true;
    }

    void deactivate() {
        active = false;
    }
};