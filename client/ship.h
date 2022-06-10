// Copyright 2022 Roman Ezhov. Github: RaEzhov

#ifndef CLIENT_SHIP_H_
#define CLIENT_SHIP_H_

#include <memory>
#include <vector>
#include <utility>
#include <iostream>

#include "client/screen_objects.h"
#include "client/game_field_cell.h"

class GameField;
class GameFieldCell;

using GameFieldCellMatrix = std::vector<std::vector<GameFieldCell>>;

ShipDirection& operator++(const ShipDirection& other);

const ShipDirection operator++(const ShipDirection& other, int);

template <char N>
class Ship : public ScreenObject {
  static_assert(N >= 1 && N <= 4);

 public:
  Ship(sf::Vector2<float> scale,
       std::pair<char, char> coordinates,
       sf::Vector2<float> position,
       std::shared_ptr<sf::RenderWindow> window_);

  ~Ship();

  void draw() const;

  void changeDirection(const std::vector<std::vector<GameFieldCell>> &cells);

  ShipDirection getDirection() const;

  std::pair<char, char> getCoords() const;

  void updateAvailability(GameFieldCellMatrix *clls, bool remove = true);

  ShipCheckStatus eventCheck(const sf::Event &event,
                             GameFieldCellMatrix *cells,
                             std::function<void(Ship<N> *ship)>
                             updateAllShipsAvailability,
                             std::function<void(
                                 std::pair<char, char> coords,
                                 char shipSize)> removeShip);

  void shoot();

  bool isAlive() const;

  static bool coordsValid(std::pair<char, char> coords) {
    return coords.first >= 0 && coords.first <= 9 &&
        coords.second >= 0 && coords.second <= 9;
  }

  static void loadTextures() {
    alive = std::make_unique<sf::Texture>();
    injured = std::make_unique<sf::Texture>();
    destroyed = std::make_unique<sf::Texture>();
    alive->loadFromFile(Config::instance().resources + "Ship" +
        std::to_string(N) + "_a.png");
    injured->loadFromFile(Config::instance().resources + "Ship" +
        std::to_string(N) + "_i.png");
    destroyed->loadFromFile(Config::instance().resources + "Ship" +
        std::to_string(N) + "_d.png");
  }

  static char aliveShips;

 private:
  static inline std::unique_ptr<sf::Texture> alive, injured, destroyed;
  sf::Sprite sprite;
  ShipState state;
  unsigned char aliveParts;
  ShipDirection direction;
  std::pair<char, char> coords;
};

template<char N>
char Ship<N>::aliveShips = 0;

template<char N>
Ship<N>::Ship(sf::Vector2<float> scale, std::pair<char, char> coordinates,
              sf::Vector2<float> position,
              std::shared_ptr<sf::RenderWindow> window_) :
    ScreenObject(window_), sprite(*alive), state(ALIVE), aliveParts(N),
    direction(UP), coords(std::move(coordinates)) {
  aliveShips++;
  sprite.setOrigin(32.f, 32.f);
  sprite.setPosition(position);
  sprite.setScale(scale);
}

template<char N>
Ship<N>::~Ship() {
  aliveShips--;
}

template<char N>
void Ship<N>::draw() const {
  window->draw(sprite);
}

template<char N>
void Ship<N>::updateAvailability(GameFieldCellMatrix *clls, bool remove) {
  switch (direction) {
    case UP:
      for (char i = -1; i <= 1; i++) {
        for (char j = -1; j <= N; j++) {
          if (coordsValid({coords.first + i, coords.second + j})) {
            if (remove) {
              (*clls)[coords.first + i][coords.second + j].rmAvailability();
            } else {
              (*clls)[coords.first + i][coords.second + j].addAvailability();
            }
            if (i == 0 && j >= 0 && j <= N - 1) {
              (*clls)[coords.first + i][coords.second + j].setUnderShip(remove);
            }
          }
        }
      }
      break;
    case RIGHT:
      for (char i = -1; i <= 1; i++) {
        for (char j = -1; j <= N; j++) {
          if (coordsValid({coords.first - j, coords.second + i})) {
            if (remove) {
              (*clls)[coords.first - j][coords.second + i].rmAvailability();
            } else {
              (*clls)[coords.first - j][coords.second + i].addAvailability();
            }
            if (i == 0 && j >= 0 && j <= N - 1) {
              (*clls)[coords.first - j][coords.second + i].setUnderShip(remove);
            }
          }
        }
      }
      break;
    case DOWN:
      for (char i = -1; i <= 1; i++) {
        for (char j = -1; j <= N; j++) {
          if (coordsValid({coords.first + i, coords.second - j})) {
            if (remove) {
              (*clls)[coords.first + i][coords.second - j].rmAvailability();
            } else {
              (*clls)[coords.first + i][coords.second - j].addAvailability();
            }
            if (i == 0 && j >= 0 && j <= N - 1) {
              (*clls)[coords.first + i][coords.second - j].setUnderShip(remove);
            }
          }
        }
      }
      break;
    case LEFT:
      for (char i = -1; i <= 1; i++) {
        for (char j = -1; j <= N; j++) {
          if (coordsValid({coords.first + j, coords.second + i})) {
            if (remove) {
              (*clls)[coords.first + j][coords.second + i].rmAvailability();
            } else {
              (*clls)[coords.first + j][coords.second + i].addAvailability();
            }
            if (i == 0 && j >= 0 && j <= N - 1) {
              (*clls)[coords.first + j][coords.second + i].setUnderShip(remove);
            }
          }
        }
      }
      break;
  }
}

template<char N>
ShipCheckStatus Ship<N>::eventCheck(const sf::Event& event,
                                    GameFieldCellMatrix *cells,
    std::function<void(Ship<N>* ship)> updateAllShipsAvailability,
    std::function<void(std::pair<char, char> coords,
        char shipSize)> removeShip) {
  updateAvailability(cells, false);
  updateAllShipsAvailability(this);
  auto rect = sprite.getGlobalBounds();
  auto scale = sprite.getScale();
  rect.width -= 32.f * scale.x;
  rect.height -= 32.f * scale.y;
  rect.left += 16.f * scale.x;
  rect.top += 16.f * scale.y;
  auto mousePos = sf::Mouse::getPosition(*window);
  if (sf::IntRect(rect).contains(mousePos)) {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
      removeShip(coords, N);
      return REMOVED;
    }
    if (event.type == sf::Event::MouseButtonReleased) {
      switch (direction) {
        case UP:
          for (char i = 0; i < N; i++) {
            if (coords.first - i < 0
                || !(*cells)[coords.first - i][coords.second].isAvailable()) {
              return NONE;
            }
          }
          break;
        case RIGHT:
          for (char i = 0; i < N; i++) {
            if (coords.second - i < 0
                || !(*cells)[coords.first][coords.second - i].isAvailable()) {
              return NONE;
            }
          }
          break;
        case DOWN:
          for (char i = 0; i < N; i++) {
            if (coords.first + i > 9
                || !(*cells)[coords.first + i][coords.second].isAvailable()) {
              return NONE;
            }
          }
          break;
        case LEFT:
          for (char i = 0; i < N; i++) {
            if (coords.second + i > 9
                || !(*cells)[coords.first][coords.second + i].isAvailable()) {
              return NONE;
            }
          }
          break;
      }
      changeDirection(*cells);
      return ROTATED;
    }
  }
  updateAvailability(cells);
  return NONE;
}

template<char N>
void Ship<N>::changeDirection(const GameFieldCellMatrix &cells) {
  sprite.rotate(90);
  direction++;
}

template<char N>
ShipDirection Ship<N>::getDirection() const {
  return direction;
}

template<char N>
std::pair<char, char> Ship<N>::getCoords() const {
  return coords;
}

template<char N>
void Ship<N>::shoot() {
  if (state != DESTROYED) {
    if (--aliveParts != 0) {
      sprite.setTexture(*injured);
    } else {
      sprite.setTexture(*destroyed);
    }
  } else {
    std::cerr << "Ship " << N << " already destroyed!\n";
  }
}

template<char N>
bool Ship<N>::isAlive() const {
  return aliveParts > 0;
}

#endif  // CLIENT_SHIP_H_
