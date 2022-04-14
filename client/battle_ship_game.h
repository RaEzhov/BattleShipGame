#ifndef BATTLE_SHIP_GAME_H
#define BATTLE_SHIP_GAME_H

#include <list>
#include <utility>
#include <iostream>

#include "screen_objects.h"


class GameFieldCell;

template <char N>
class Ship;


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

enum ShipDirection{
    UP,
    RIGHT,
    DOWN,
    LEFT
};

enum ShipCheckStatus{
    REMOVED,
    ROTATED,
    NONE
};


ShipDirection& operator++(ShipDirection& other);

const ShipDirection operator++(ShipDirection& other, int);


class DraggableAndDroppableShips: public ScreenObject {
public:
    DraggableAndDroppableShips(const sf::Vector2<float> &scale_, const sf::VideoMode &screen,
                               std::shared_ptr<sf::RenderWindow> window_);

    void draw() const;

    void eventCheck(sf::Event &event);

private:
    std::array<sf::Texture, 5> tShip;
    std::array<sf::Sprite, 5> sShip;
    std::array<sf::Vector2<float>, 5> startPos, tempPos;
    std::array<bool, 5> dragged{};
    sf::Vector2<float> scale;
};


class GameField: public ScreenObject {
public:
    GameField(sf::Vector2<float> position_, sf::Vector2<float> scale, GameFieldState state_,
              std::shared_ptr<sf::RenderWindow> window_);

    void eventCheck(sf::Event &event);

    void draw() const;

    bool addShip(char i, char j, char shipType);

    void removeShip(std::pair<char, char> coords, char shipType);

    template <char N>
    void updateAvailability(Ship<N>& ship);

    void clearAvailability();

    std::vector<GameFieldCell>& operator[](size_t i);

    /**Size of dragged ship for DraggableAndDroppableShip class*/
    static char shipSize;

    /**Difference between cursor and upper left ship corner on y-axis*/
    static float diffY;

private:
    template<char N>
    void drawShips(const std::list<Ship<N>> &ships) const;

private:
    Picture field;
    std::list<Ship<1>> ship1;
    std::list<Ship<2>> ship2;
    std::list<Ship<3>> ship3;
    std::list<Ship<4>> ship4;
    unsigned char aliveCount;
    sf::Vector2<float> scale, position;
    GameFieldState state;

    std::vector<std::vector<GameFieldCell>> cells;
};

class GameFieldCell : public ScreenObject {
public:
    GameFieldCell(sf::Vector2<float> scale, sf::Vector2<float> position, std::shared_ptr<sf::RenderWindow> window_);

    void setPosition(sf::Vector2<float> newPosition);

    void eventCheck(sf::Event& event, GameFieldState state, GameField& parent,
                    char i, char j, sf::Vector2<float> scale);

    void draw() const;

    void setAlpha(unsigned char alpha);

    bool isAvailable() const;

    void rmAvailability();

    void addAvailability();

private:
    sf::RectangleShape cell;
    bool availability;
};

template <char N>
class Ship : public ScreenObject {
    static_assert(N >= 1 and N <= 4);
public:
    Ship(sf::Vector2<float> scale, std::pair<char, char> coordinates, sf::Vector2<float> position, std::shared_ptr<sf::RenderWindow> window_) :
            ScreenObject(window_), sprite(alive[N]), state(ALIVE), aliveParts(N), direction(UP), coords(std::move(coordinates)) {
        sprite.setOrigin(32.f, 32.f);
        sprite.setPosition(position);
        sprite.setScale(scale);
    }

    void draw() const {
        window->draw(sprite);
    }

    static bool coordsValid(std::pair<char, char> coords){
        return coords.first >= 0 && coords.first <= 9 && coords.second >= 0 && coords.second <= 9;
    }

    void changeDirection(std::vector<std::vector<GameFieldCell>>& cells){
        sprite.rotate(90);
        direction++;
    }

    ShipDirection getDirection() const{
        return direction;
    }

    const std::pair<char, char>& getCoords() const{
        return coords;
    }

    /**Updates availability for this ship*/
    void updateAvailability(std::vector<std::vector<GameFieldCell>>& cells, bool remove = true){
        switch (direction) {
            case UP:
                for (char i = -1; i <= 1; i++){
                    for (char j = -1; j <= N; j++){
                        if (coordsValid({coords.first + i, coords.second + j})){
                            if (remove) {
                                cells[coords.first + i][coords.second + j].rmAvailability();
                            } else {
                                cells[coords.first + i][coords.second + j].addAvailability();
                            }
                        }
                    }
                }
                break;
            case RIGHT:
                for (char i = -1; i <= 1; i++){
                    for (char j = -1; j <= N; j++){
                        if (coordsValid({coords.first - j, coords.second + i})){
                            if (remove) {
                                cells[coords.first - j][coords.second + i].rmAvailability();
                            } else {
                                cells[coords.first - j][coords.second + i].addAvailability();
                            }
                        }
                    }
                }
                break;
            case DOWN:
                for (char i = -1; i <= 1; i++){
                    for (char j = -1; j <= N; j++){
                        if (coordsValid({coords.first + i, coords.second - j})){
                            if (remove) {
                                cells[coords.first + i][coords.second - j].rmAvailability();
                            } else {
                                cells[coords.first + i][coords.second - j].addAvailability();
                            }
                        }
                    }
                }
                break;
            case LEFT:
                for (char i = -1; i <= 1; i++){
                    for (char j = -1; j <= N; j++){
                        if (coordsValid({coords.first + j, coords.second + i})){
                            if (remove) {
                                cells[coords.first + j][coords.second + i].rmAvailability();
                            } else {
                                cells[coords.first + j][coords.second + i].addAvailability();
                            }
                        }
                    }
                }
                break;
        }
    }

    ShipCheckStatus eventCheck(sf::Event& event, std::vector<std::vector<GameFieldCell>>& cells,
                          std::function<void(Ship<N>& ship)> updateAllShipsAvailability, GameField& field) {
        updateAvailability(cells, false);
        updateAllShipsAvailability(*this);
        auto rect = sprite.getGlobalBounds();
        auto scale = sprite.getScale();
        rect.width  -= 32.f * scale.x;
        rect.height -= 32.f * scale.y;
        rect.left   += 16.f * scale.x;
        rect.top    += 16.f * scale.y;
        auto mousePos = sf::Mouse::getPosition(*window);
        if (sf::IntRect(rect).contains(mousePos)) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Right)){
                field.removeShip(coords, N);
                return REMOVED;
            }
            if (event.type == sf::Event::MouseButtonReleased) {
                switch (direction) {
                    case UP:
                        for (char i = 0; i < N; i++) {
                            if (coords.first - i < 0 || !cells[coords.first - i][coords.second].isAvailable()) {
                                return NONE;
                            }
                        }
                        break;
                    case RIGHT:
                        for (char i = 0; i < N; i++) {
                            if (coords.second - i < 0 || !cells[coords.first][coords.second - i].isAvailable()) {
                                return NONE;
                            }
                        }
                        break;
                    case DOWN:
                        for (char i = 0; i < N; i++) {
                            if (coords.first + i > 9 || !cells[coords.first + i][coords.second].isAvailable()) {
                                return NONE;
                            }
                        }
                        break;
                    case LEFT:
                        for (char i = 0; i < N; i++) {
                            if (coords.second + i > 9 || !cells[coords.first][coords.second + i].isAvailable()) {
                                return NONE;
                            }
                        }
                        break;
                }
                changeDirection(cells);
                return ROTATED;
            }
        }
        updateAvailability(cells);
        return NONE;
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
    ShipDirection direction;
    std::pair<char, char> coords;
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
