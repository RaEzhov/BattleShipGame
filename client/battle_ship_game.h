#ifndef BATTLE_SHIP_GAME_H
#define BATTLE_SHIP_GAME_H

#include "screen_objects.h"
#include "game_field.h"

enum UserStatus {
    LOGIN,
    MAIN_MENU,
    GET,
    IN_SP_MENU,
    IN_SP_GAME,
    IN_MP_MENU,
    IN_MP_GAME
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

    void startBattle(bool);

private:
    std::unique_ptr<sf::TcpSocket> server;
    std::shared_ptr<sf::RenderWindow> window;
    sf::VideoMode screen;
    sf::Vector2<float> screenScale;
    std::unordered_map<std::string, std::unique_ptr<Button>> buttons;
    std::unordered_map<std::string, std::unique_ptr<Entry>> entries;
    std::unordered_map<std::string, std::unique_ptr<Picture>> pictures;
    std::unordered_map<std::string, std::unique_ptr<Title>> titles;
    std::unordered_map<std::string, std::unique_ptr<DraggableAndDroppableShips>> dragDropShips;
    std::unordered_map<std::string, std::unique_ptr<GameField>> fields;

    sf::Texture cursorTex;
    sf::Sprite cursor;

    UserStatus status;

    static const float WIDTH, HEIGHT;
};

#endif//BATTLE_SHIP_GAME_H
