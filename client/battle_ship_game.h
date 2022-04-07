#ifndef BATTLE_SHIP_GAME_H
#define BATTLE_SHIP_GAME_H

#include "screen_objects.h"

enum UserStatus{
    LOGIN = 1,
    MAIN_MENU = 2,
    GET = 3,
    IN_SP_MENU = 4,
    IN_SP_GAME = 5
};


class BattleShipGame final {
public:
    BattleShipGame();

    void mainLoop();

private:
    void loadTextures();

    void mainMenu();

    void loginFunc();

    void singlePlayerFunc();

    void multiPlayerFunc();

private:
    std::unique_ptr<sf::TcpSocket> server;
    std::shared_ptr<sf::RenderWindow> window;
    sf::VideoMode screen;
    sf::Vector2<float> screenScale;
    std::unordered_map<std::string, std::unique_ptr<Button>> buttons;
    std::unordered_map<std::string, std::unique_ptr<Entry>> entries;
    std::unordered_map<std::string, std::unique_ptr<Picture>> pictures;
    std::unordered_map<std::string, std::unique_ptr<Title>> titles;

    sf::Texture cursorTex;
    sf::Sprite cursor;

    UserStatus status;

    static const float WIDTH, HEIGHT;
};

#endif//BATTLE_SHIP_GAME_H
