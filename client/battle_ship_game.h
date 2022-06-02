#ifndef BATTLE_SHIP_GAME_H
#define BATTLE_SHIP_GAME_H

#include "screen_objects.h"
#include "game_field.h"
#include "user.h"

class BattleShipGame final {
public:
    BattleShipGame();

    void mainLoop();

    static std::pair<unsigned char, unsigned char> moveCoords;

    static bool sendMove;

private:
    void loadTextures();

    void mainMenu();

    void loginFunc();

    void registerFunc();

    void singlePlayerFunc();

    void multiPlayerLobby();

    void setEnemyTitles();

    void multiPlayerFunc(const std::string &enemy);

    void addFriend();

    void removeFriend();

    void randomRival();

    void startBattle();

    void finishBattle(bool);

    void changeSide();

    [[noreturn]] void serverListener();

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
    std::unordered_map<std::string, std::unique_ptr<Pages>> pages;
    std::unique_ptr<NotificationPool> notifications;

    sf::Texture cursorTex;
    sf::Sprite cursor;

    User user, enemy;

    static const float WIDTH, HEIGHT;
};

#endif//BATTLE_SHIP_GAME_H
