#include "screens.h"
#include "tile.h"
#include <iostream>
#include <string>
#include <fstream>
#include <SFML/Graphics.hpp>

using namespace std;

int main() {
    // Store all textures
    map<string, sf::Texture> textures;
    vector<string> textureNames = {"debug", "digits", "face_happy", "face_lose", "face_win", "flag", "leaderboard",
                                   "mine", "number_1", "number_2", "number_3", "number_4", "number_5", "number_6",
                                   "number_7", "number_8", "pause", "play", "tile_hidden", "tile_revealed"};
    string path = "files/images/";
    string type = ".png";
    for (const auto& textureName : textureNames) {
        sf::Texture texture;
        if (!texture.loadFromFile(path + textureName + type)) {
            cout << "Error loading texture: " << textureName << endl;
        }
        textures[textureName] = texture;
    }

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
    GameScreen gameScreen(window, width, height, numRows, numColumns, numMines, textures);

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
                    else if (event.key.code == sf::Keyboard::Enter && welcomeScreen.name.length() > 1) {
                        welcomeScreen.deactivate();
                        gameScreen.activate();
                    }
                }
            }
            // The logic for playing the game.
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