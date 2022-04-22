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
              std::shared_ptr<sf::RenderWindow> window_, std::function<void()> changeSide_);

    void eventCheck(sf::Event &event);

    void draw() const;

    bool addShip(char i, char j, char shipType);

    void removeShip(std::pair<char, char> coords, char shipType);

    void setState(GameFieldState newState);

    void placeShipsRand();

    void clearShips();

    void selfMove(){
        sf::sleep(sf::seconds(0.1f));
        char i = 0, j = 0;
        for (auto& v: cells){
            for (auto& c: v){
                if (c.isAvailable()){
                    c.shoot();
                    if (c.underShip) {
                        findShip(std::pair<char, char>(i, j));
                    }
                    changeSide();
                    return;
                }
                j++;
            }
            i++;
            j = 0;
        }
    }

    template<char N>
    void shoot(std::pair<char, char> coords, Ship<N>& ship) {
        for (int i = 0; i < N; i++) {
            switch (ship.getDirection()) {
                case UP:
                    if (coords.first == ship.getCoords().first &&
                        coords.second == ship.getCoords().second + i){
                        ship.shoot();
                        return;
                    }
                    break;
                case RIGHT:
                    if (coords.first == ship.getCoords().first - i &&
                        coords.second == ship.getCoords().second){
                        ship.shoot();
                        return;
                    }
                    break;
                case DOWN:
                    if (coords.first == ship.getCoords().first &&
                        coords.second == ship.getCoords().second - i){
                        ship.shoot();
                        return;
                    }
                    break;
                case LEFT:
                    if (coords.first == ship.getCoords().first + i &&
                        coords.second == ship.getCoords().second + i){
                        ship.shoot();
                        return;
                    }
                    break;
            }
        }
    }

    void findShip(std::pair<char, char> coords) {
        for (auto& s: ship1){
            shoot(coords, s);
        }
        for (auto& s: ship2){
            shoot(coords, s);
        }
        for (auto& s: ship3){
            shoot(coords, s);
        }
        for (auto& s: ship4){
            shoot(coords, s);
        }
    }

    template <char N>
    void updateAvailability(Ship<N>& ship);

    void clearAvailability(bool onlyAvailability = false);

    std::vector<GameFieldCell>& operator[](size_t i);

    /**Size of dragged ship for DraggableAndDroppableShip class*/
    static char shipSize;

    /**Difference between cursor and upper left ship corner on y-axis*/
    static float diffY;

    std::function<void()> changeSide;

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
