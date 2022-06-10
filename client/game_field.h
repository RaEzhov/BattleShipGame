// Copyright 2022 Roman Ezhov. Github: RaEzhov

#ifndef CLIENT_GAME_FIELD_H_
#define CLIENT_GAME_FIELD_H_

#include <memory>
#include <utility>
#include <list>
#include <vector>
#include <array>

#include "client/screen_objects.h"
#include "client/ship.h"

class DragNDropShp: public ScreenObject {
 public:
  DragNDropShp(const sf::Vector2<float> &scale_, const sf::VideoMode &screen,
               std::shared_ptr<sf::RenderWindow> window_);

  void draw() const;

  void eventCheck(const sf::Event &event);

 private:
  std::array<sf::Texture, 5> tShip;
  std::array<sf::Sprite, 5> sShip;
  std::array<sf::Vector2<float>, 5> startPos, tempPos;
  std::array<bool, 5> dragged;
  sf::Vector2<float> scale;
};

class GameField : public ScreenObject {
 public:
  friend class BattleShipGame;

  friend class GameFieldCell;

  GameField(sf::Vector2<float> position_,
            sf::Vector2<float> scale,
            GameFieldState state_,
            std::shared_ptr<sf::RenderWindow> window_,
            std::function<void()> changeSide_);

  void eventCheck(const sf::Event &event);

  void draw() const;

  bool addShip(char i, char j, char shipType);

  void removeShip(std::pair<char, char> coords, char shipType);

  void setState(GameFieldState newState);

  void placeShipsRand();

  void clearShips();

  std::vector<sf::Uint16> serializedField();

  void selfMove(std::pair<unsigned char, unsigned char> move = {100, 100});

  void clearColors();

  template<char N>
  void shoot(std::pair<char, char> coords, Ship<N> *ship);

  void findShip(std::pair<char, char> coords);

  template<char N>
  void updateAvailability(Ship<N> *ship);

  void clearAvailability(bool onlyAvailability = false);

  std::vector<GameFieldCell> &operator[](size_t i);

  /**Size of dragged ship for DraggableAndDroppableShip class*/
  static char shipSize;

  /**Difference between cursor and upper left ship corner on y-axis*/
  static float diffY;

  std::function<void()> changeSide;

  unsigned char getAliveShips() const;

 private:
  template<char N>
  void drawShips(const std::list<Ship<N>> &ships) const;

 private:
  Picture field, border;
  std::list<Ship<1>> ship1;
  std::list<Ship<2>> ship2;
  std::list<Ship<3>> ship3;
  std::list<Ship<4>> ship4;
  sf::Vector2<float> scale, position;
  GameFieldState state;

  GameFieldCellMatrix cells;
};

#endif  // CLIENT_GAME_FIELD_H_
