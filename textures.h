#pragma once

class Textures {
public:
    // Store all textures
    map<string, sf::Texture> textures;
    vector<string> textureNames = {"debug", "digits", "face_happy", "face_lose", "face_win", "flag", "leaderboard",
                                   "mine", "number_1", "number_2", "number_3", "number_4", "number_5", "number_6",
                                   "number_7", "number_8", "pause", "play", "tile_hidden", "tile_revealed"};
    string path = "files/images/";
    string type = ".png";

    Textures() {
        for (const auto& textureName : textureNames) {
            sf::Texture texture;
            if (!texture.loadFromFile(path + textureName + type)) {
                cout << "Error loading texture: " << textureName << endl;
            }
            textures[textureName] = texture;
        }
    }

    sf::Texture& getTexture(const string& textureName) {
        return textures[textureName];
    }
};