// Copyright 2022 Roman Ezhov. Github: RaEzhov

#ifndef CLIENT_GAME_FIELD_CELL_H_
#define CLIENT_GAME_FIELD_CELL_H_

#include <memory>

#include "client/screen_objects.h"

class GameField;

class GameFieldCell : public ScreenObject {
 public:
  friend GameField;

  GameFieldCell(sf::Vector2<float> scale,
                sf::Vector2<float> position,
                std::shared_ptr<sf::RenderWindow> window_);

  void setPosition(sf::Vector2<float> newPosition);

  void eventCheck(const sf::Event &event, GameFieldState state,
                  GameField *parent, char i, char j,
                  sf::Vector2<float> scale);

  void shoot();

  void setUnderShip(bool under);

  void draw() const;

  void setAlpha(unsigned char alpha);

  bool isAvailable() const;

  void rmAvailability();

  void addAvailability();

  bool isUnderShip() const;

 private:
  sf::RectangleShape cell;
  bool availability;
  bool underShip;
};

#endif  // CLIENT_GAME_FIELD_CELL_H_
