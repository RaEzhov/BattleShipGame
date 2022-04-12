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

enum ShipDirection{
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3
};

class GameField;

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

class GameFieldCell : public ScreenObject {
public:
    GameFieldCell(sf::Vector2<float> scale, sf::Vector2<float> position, std::shared_ptr<sf::RenderWindow> window_);

    void setPosition(sf::Vector2<float> newPosition);

    void eventCheck(sf::Event& event, GameFieldState state, GameField& parent,
                    size_t i, size_t j, sf::Vector2<float> scale);

    void draw() const;

    void setAlpha(unsigned char alpha) {
        cell.setFillColor(sf::Color(255, 255, 255, alpha));
    }

    bool isAvailable() const{
        return availability;
    }

    void rmAvailability(){
        availability = false;
    }

private:
    sf::RectangleShape cell;
    bool availability;
};

template <char N>
class Ship : public ScreenObject {
    static_assert(N >= 1 and N <= 4);
public:
    Ship(sf::Vector2<float> scale, sf::Vector2<float> position, std::shared_ptr<sf::RenderWindow> window_) :
            ScreenObject(window_), sprite(alive[N]), state(ALIVE), aliveParts(N), direction(UP) {
        sprite.setPosition(position);
        sprite.setScale(scale);
    }

    void draw() const {
        window->draw(sprite);
    }

    void changeDirection(){
        direction++;
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
};

class GameField: public ScreenObject {
public:
    GameField(sf::Vector2<float> position_, sf::Vector2<float> scale, GameFieldState state_,
              std::shared_ptr<sf::RenderWindow> window_);

    void eventCheck(sf::Event &event);

    void draw() const;

    bool addShip(size_t i, size_t j, char shipType) {
        if (cells[i][j].isAvailable()) {
            switch (shipType) {
                case 1:
                    if (cells[i][j].isAvailable()){
                        ship1.emplace_back(scale, sf::Vector2<float>(position.x + (33.f * static_cast<float>(i) - 16.f) * scale.x,
                                                                     position.y + (33.f * static_cast<float>(j) - 16.f) * scale.y),
                                           window);
                        for (int i_ = -1; i_ <= 1; i_++) {
                            for (int j_ = -1; j_ <= 1; j_++) {
                                if (i + i_ >= 0 && i + i_ <= 9 &&
                                    j + j_ >= 0 && j + j_ <= 9) {
                                    cells[i + i_][j + j_].rmAvailability();
                                }
                            }
                        }
                    } else {
                        return false;
                    }
                    break;
                case 2:
                    if (j <= 8 && cells[i][j].isAvailable() && cells[i][j+1].isAvailable()){
                        ship2.emplace_back(scale, sf::Vector2<float>(position.x + (33.f * static_cast<float>(i) - 16.f) * scale.x,
                                                                     position.y + (33.f * static_cast<float>(j) - 16.f) * scale.y),
                                           window);
                        for (int i_ = -1; i_ <= 1; i_++) {
                            for (int j_ = -1; j_ <= 2; j_++) {
                                if (i + i_ >= 0 && i + i_ <= 9 &&
                                    j + j_ >= 0 && j + j_ <= 9) {
                                    cells[i + i_][j + j_].rmAvailability();
                                }
                            }
                        }
                    } else {
                        return false;
                    }
                    break;
                case 3:
                    if (j <= 7 && cells[i][j].isAvailable() && cells[i][j+1].isAvailable() && cells[i][j+2].isAvailable()){
                        ship3.emplace_back(scale, sf::Vector2<float>(position.x + (33.f * static_cast<float>(i) - 16.f) * scale.x,
                                                                     position.y + (33.f * static_cast<float>(j) - 16.f) * scale.y),
                                           window);
                        for (int i_ = -1; i_ <= 1; i_++) {
                            for (int j_ = -1; j_ <= 3; j_++) {
                                if (i + i_ >= 0 && i + i_ <= 9 &&
                                    j + j_ >= 0 && j + j_ <= 9) {
                                    cells[i + i_][j + j_].rmAvailability();
                                }
                            }
                        }
                    } else {
                        return false;
                    }
                    break;
                case 4:
                    if (j <= 6 && cells[i][j].isAvailable() && cells[i][j+1].isAvailable() && cells[i][j+2].isAvailable() && cells[i][j+3].isAvailable()){
                        ship4.emplace_back(scale, sf::Vector2<float>(position.x + (33.f * static_cast<float>(i) - 16.f) * scale.x,
                                                                     position.y + (33.f * static_cast<float>(j) - 16.f) * scale.y),
                                           window);
                        for (int i_ = -1; i_ <= 1; i_++) {
                            for (int j_ = -1; j_ <= 4; j_++) {
                                if (i + i_ >= 0 && i + i_ <= 9 &&
                                    j + j_ >= 0 && j + j_ <= 9) {
                                    cells[i + i_][j + j_].rmAvailability();
                                }
                            }
                        }
                    } else {
                        return false;
                    }
                    break;
                default:
                    std::cerr << "Wrong ship type!\n";
                    break;
            }
            return true;
        }
        return false;
    }
    std::vector<GameFieldCell>& operator[](size_t i){
        if (i <= cells.size()) {
            return cells[i];
        }
        throw std::runtime_error("Invalid index in GameField!\n");
    }

    /**Size of dragged ship for DraggableAndDroppableShip class*/
    static char shipSize;

    /**Difference between cursor and upper left ship corner on y-axis*/
    static float diffY;

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
    sf::Vector2<float> scale, position;
    GameFieldState state;

    std::vector<std::vector<GameFieldCell>> cells;
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
