#include "screens.h"
#include "textures.h"
#include "leaderboard.h"
#include <iostream>
#include <string>
#include <fstream>
#include <SFML/Graphics.hpp>

using namespace std;

int main() {
    Textures textures;

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

    // Create game screen
    GameScreen gameScreen(window, width, height, numRows, numColumns, numMines, textures.textures);

    // Create leaderboard screen
    int leaderWidth = (numColumns * 16);
    int leaderHeight = (numRows * 16) + 50;
    sf::RenderWindow leaderboardWindow(sf::VideoMode(leaderWidth, leaderHeight), "Leaderboard", sf::Style::Close);
    Leaderboard leaderboard(window, leaderWidth, leaderHeight);
    leaderboardWindow.setVisible(false);

    // Main Game Loop
    while(window.isOpen()) {

        // Event Checker
        sf::Event event;
        while(window.pollEvent(event)) {
            // Exits the program when the 'X' is clicked.
            if(event.type == sf::Event::Closed && !leaderboard.active) {
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
                        gameScreen.name = welcomeScreen.name;
                    }
                }
            }
            // The logic for playing the game.
            else if (gameScreen.active && !leaderboard.active) {
                // Left-clicks
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                {
                    int mouseX = event.mouseButton.x;
                    int mouseY = event.mouseButton.y;

                    // Left-clicks on the face button while the game is active, restart the game.
                    if (gameScreen.happyFaceButton.getGlobalBounds().contains((float)mouseX, (float)mouseY)) {
                        gameScreen.reset();
                        leaderboard.resetLeaderboard(gameScreen.newRank);
                    }
                    // Left-clicks on the debug button toggles debug mode.
                    else if (gameScreen.debugButton.getGlobalBounds().contains((float)mouseX, (float)mouseY)) {
                        gameScreen.toggleDebugMode();
                    }
                    // Left-clicks on the pause button when the game is active but paused will unpause the game.
                    else if (!gameScreen.isNewGame && gameScreen.isPaused && gameScreen.pauseButton.getGlobalBounds().contains((float)mouseX, (float)mouseY)) {
                        gameScreen.togglePause();
                    }
                    // Left-clicks on the pause button when the game is paused will resume the game.
                    else if (!gameScreen.isPaused && gameScreen.pauseButton.getGlobalBounds().contains((float)mouseX, (float)mouseY)) {
                        gameScreen.togglePause();
                    }
                    // Left-clicks on the leaderboard button will show the leaderboard window
                    else if (gameScreen.leaderButton.getGlobalBounds().contains((float)mouseX, (float)mouseY)) {

                        // Pause active games
                        if (!gameScreen.isPaused) {
                            gameScreen.pause();
                            leaderboard.resumeAfterClose = true;        // Resume after the leaderboard closes
                        }

                        // Show the leaderboard window
                        leaderboard.active = true;
                        leaderboardWindow.setVisible(true);
                    }
                    else {
                        // Left-clicks on the board
                        gameScreen.leftClickAction(mouseX, mouseY);
                    }
                }
                // Right-clicks
                else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
                    // If the user right-clicks a hidden tile, flag it.
                    gameScreen.rightClickAction(event.mouseButton.x, event.mouseButton.y);
                }

                // If the game has been won after clicking, switch to the leaderboard
                if (gameScreen.gameWon && !gameScreen.leaderboardShownAtEndGame) {
                    leaderboard.resetLeaderboard(gameScreen.newRank);
                    leaderboard.active = true;
                    leaderboardWindow.setVisible(true);
                }
            }
        }

        // Draw welcome screen
        window.clear();
        if (welcomeScreen.active) {
            welcomeScreen.drawToScreen(window);
        }

        // Draw game screen
        if (gameScreen.active) {
            gameScreen.drawToScreen(window);
        }
        window.display();

        sf::Event leaderboardEvent;
        while(leaderboardWindow.pollEvent(leaderboardEvent)) {
            // Exits the program when the 'X' is clicked.
            if (leaderboardEvent.type == sf::Event::Closed) {
                leaderboardWindow.setVisible(false);
                leaderboard.active = false;

                // Resume the game if the leaderboard originally came up when the game was active
                if (leaderboard.resumeAfterClose) {
                    leaderboard.resumeAfterClose = false;
                    gameScreen.unpause();
                }
                // If the game was won on the last click, then
                else if (gameScreen.gameWon) {
                    leaderboardWindow.setVisible(false);
                    leaderboard.active = false;
                    gameScreen.leaderboardShownAtEndGame = true;
                }
            }
        }

        if (leaderboard.active) {
            leaderboardWindow.clear();
            leaderboard.drawToScreen(leaderboardWindow);
            leaderboardWindow.display();
        }

    }

    return 0;
}