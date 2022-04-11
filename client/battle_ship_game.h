#ifndef BATTLE_SHIP_GAME_H
#define BATTLE_SHIP_GAME_H

#include <utility>
#include <iostream>

#include "screen_objects.h"

enum UserStatus {
    LOGIN = 1,
    MAIN_MENU = 2,
    GET = 3,
    IN_SP_MENU = 4,
    IN_SP_GAME = 5
};

enum ShipState {
    ALIVE,
    INJURED,
    DESTROYED
};

enum GameFieldState{
    PLACEMENT,
    INACTIVE,
    GAME
};

class DraggableAndDroppableShips: public ScreenObject {
public:
    DraggableAndDroppableShips(const sf::Vector2<float> &scale, const sf::VideoMode &screen,
                               std::shared_ptr<sf::RenderWindow> window_);

    void draw() const;

    void eventCheck(sf::Event &event);

private:
    std::array<sf::Texture, 5> tShip;
    std::array<sf::Sprite, 5> sShip;
    std::array<sf::Vector2<float>, 5> startPos, tempPos;
    std::array<bool, 5> dragged{};
};

class GameFieldCell : public ScreenObject {
public:
    GameFieldCell(sf::Vector2<float> scale, sf::Vector2<float> position, std::shared_ptr<sf::RenderWindow> window_);

    void setPosition(sf::Vector2<float> newPosition);

    void eventCheck(sf::Event &event);

    void draw() const;

private:
    sf::RectangleShape cell;
    bool underShip;

};

template <char N>
class Ship : public ScreenObject {
    static_assert(N >= 1 and N <= 4);
public:
    Ship(sf::Vector2<float> scale, sf::Vector2<float> position, std::shared_ptr<sf::RenderWindow> window_) :
            ScreenObject(window_), sprite(alive[N]), state(ALIVE), aliveParts(N) {
        sprite.setPosition(position);
        sprite.setScale(scale);
    }

    void draw() const {
        window->draw(sprite);
    }

    void shoot() {
        if (state != DESTROYED) {
            if (--aliveParts != 0) {
                sprite.setTexture(injured[N]);
            } else {
                sprite.setTexture(destroyed[N]);
            }
        } else {
            std::cerr << "Ship" << N << " already destroyed!\n";
        }
    }

    static void loadTextures() {
        alive[N].loadFromFile(std::string(RESOURCES_PATH) + "Ship" + std::to_string(N) + "_a.png");
        injured[N].loadFromFile(std::string(RESOURCES_PATH) + "Ship" + std::to_string(N) + "_i.png");
        destroyed[N].loadFromFile(std::string(RESOURCES_PATH) + "Ship" + std::to_string(N) + "_d.png");
    }

private:
    static std::array<sf::Texture, 5> alive, injured, destroyed;
    sf::Sprite sprite;
    ShipState state;
    unsigned char aliveParts;
};

class GameField: public ScreenObject {
public:
    friend DraggableAndDroppableShips;

    GameField(sf::Vector2<float> position, sf::Vector2<float> scale, GameFieldState state_,
              std::shared_ptr<sf::RenderWindow> window_);

    void eventCheck(sf::Event &event);

    void draw() const;

private:
    template<char N>
    void drawShips(const std::vector<Ship<N>> &ships) const;

private:
    Picture field;
    std::vector<Ship<1>> ship1;
    std::vector<Ship<2>> ship2;
    std::vector<Ship<3>> ship3;
    std::vector<Ship<4>> ship4;
    unsigned char aliveCount;

    GameFieldState state;

    std::vector<std::vector<GameFieldCell>> cells;

    /**Size of dragged ship for DraggableAndDroppableShip class*/
    static char shipSize;
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
