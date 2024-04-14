#include "main.h"
#include <iostream>
#include <string>
#include <fstream>
#include <SFML/Graphics.hpp>

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
    sf::RectangleShape gameBackground;

    // Constructor
    GameScreen(sf::RenderWindow& window, int width, int height) {
        _width = width;
        _height = height;
        createGameScreen();
    }

    void createGameScreen() {
        // Testing
        setGameBackground(_width, _height, sf::Color::Red);
    }

    void setGameBackground(int width, int height, sf::Color color) {
        gameBackground.setSize(sf::Vector2f((float)width, (float)height));
        gameBackground.setFillColor(color);
    }

    // Gets the username
    void drawToScreen(sf::RenderWindow& window) {
        window.draw(gameBackground);
    }

    void activate() {
        active = true;
    }

    void deactivate() {
        active = false;
    }
};


int main() {
    // Determine size of the window through config file data
    ifstream infile("files/config.cfg");
    if (!infile) {
        cout << "Config file failed to open." << endl;
        return 1;
    }
    string line;
    getline(infile, line);
    int numColumns = stoi(line);
    getline(infile, line);
    int numRows = stoi(line);
    getline(infile, line);
    int numMines = stoi(line);
    int width = numColumns * 32;
    int height = (numRows * 32) + 100;

    // Main window
    sf::RenderWindow window(sf::VideoMode(width, height), "Minesweeper", sf::Style::Close);

    // Create the welcome screen
    WelcomeScreen welcomeScreen(window, width, height);

    // Create game window
    GameScreen gameScreen(window, width, height);

    // Main Game Loop
    while(window.isOpen()) {

        // Event Checker
        sf::Event event;
        while(window.pollEvent(event)) {
            // Exits the program when the 'X' is clicked.
            if(event.type == sf::Event::Closed) {
                window.close();
            }

            // When user enters a character on the welcome screen
            if (welcomeScreen.active) {
                if (event.type == sf::Event::TextEntered) {
                    // If the name is currently 10 characters long, ignore the keypress.
                    if (welcomeScreen.name.length() == 10) {
                        cout << "Names are maximum 10 characters." << endl;
                    }
                    // The name must be alphabetical to update the name.
                    else if (event.text.unicode < 128) {
                        char character = static_cast<char>(event.text.unicode);
                        if (isalpha(character)) {
                            welcomeScreen.addCharacter(character);
                            cout << character << endl;
                        }
                    }
                }
                // If backspace pressed, pop last character from name.
                else if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::BackSpace && welcomeScreen.name.length() > 1) {
                        welcomeScreen.popCharacter();
                    }
                    else if (event.key.code == sf::Keyboard::Enter && welcomeScreen.name.length() > 0) {
                        welcomeScreen.deactivate();
                        gameScreen.activate();
                    }
                }
            }
            else if (gameScreen.active) {

            }
        }

        // Draw the objects to the screen.
        window.clear();
        if (welcomeScreen.active) {
            welcomeScreen.drawToScreen(window);
        }
        else if (gameScreen.active) {
            gameScreen.drawToScreen(window);
        }
        window.display();
    }

    return 0;
}