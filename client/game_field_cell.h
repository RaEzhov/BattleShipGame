#ifndef GAME_FIELD_CELL_H
#define GAME_FIELD_CELL_H

#include "screen_objects.h"

class GameField;

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

#endif//GAME_FIELD_CELL_H
