#ifndef GAME_FIELD_H
#define GAME_FIELD_H

#include "screen_objects.h"
#include "ship.h"

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
    std::array<bool, 5> dragged;
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

    void setState(GameFieldState newState);

    void placeShipsRand();

    void clearShips();

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
    Picture field, border;
    std::list<Ship<1>> ship1;
    std::list<Ship<2>> ship2;
    std::list<Ship<3>> ship3;
    std::list<Ship<4>> ship4;
    unsigned char aliveCount;
    sf::Vector2<float> scale, position;
    GameFieldState state;

    std::vector<std::vector<GameFieldCell>> cells;
};

#endif//GAME_FIELD_H
