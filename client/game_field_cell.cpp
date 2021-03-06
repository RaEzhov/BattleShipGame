// Copyright 2022 Roman Ezhov. Github: RaEzhov

#include <utility>

#include "client/game_field_cell.h"
#include "client/battle_ship_game.h"

GameFieldCell::GameFieldCell(sf::Vector2<float> scale,
                             sf::Vector2<float> position,
                             std::shared_ptr<sf::RenderWindow> window_) :
    ScreenObject(std::move(window_)), availability(true), underShip(false) {
  cell.setSize({32, 32});
  cell.setScale(scale);
  cell.setFillColor(sf::Color(255, 255, 255, 0));
  cell.setPosition(position);
}

void GameFieldCell::setPosition(sf::Vector2<float> newPosition) {
  cell.setPosition(newPosition);
}

void GameFieldCell::eventCheck(const sf::Event &event,
                               GameFieldState state,
                               GameField *parent,
                               char i,
                               char j,
                               sf::Vector2<float> scale) {
  auto mouse = sf::Mouse::getPosition(*window);
  auto cellRect = sf::IntRect(cell.getGlobalBounds());
  switch (state) {
    case GAME:
      if (availability) {
        if (cellRect.contains(mouse)) {
          setAlpha(100);
          if (event.type == sf::Event::MouseButtonReleased) {
            shoot();
            if (underShip) {
              parent->findShip(std::pair<char, char>(i, j));
            }
            parent->changeSide();
            BattleShipGame::moveCoords = {i, j};
            BattleShipGame::sendMove = true;
          }
        } else {
          setAlpha(0);
        }
      }
      break;
    case PLACEMENT:
      switch (GameField::shipSize) {
        case 1:
          if (event.type == sf::Event::MouseMoved
              || event.type == sf::Event::MouseButtonReleased) {
            if (GameField::diffY <= 32.f * scale.y) {
              if (cellRect.contains(mouse)) {
                if (event.type == sf::Event::MouseButtonReleased) {
                  if (cellRect.contains(mouse)) {
                    if (parent->ship1.size() < 4) {
                      parent->addShip(i, j, GameField::shipSize);
                    }
                    for (int j_ = 0; j_ <= 0; j_++) {
                      if (j + j_ >= 0 && j + j_ <= 9) {
                        (*parent)[i][j + j_].setAlpha(0);
                      }
                    }
                  }
                } else {
                  if (cellRect.contains(mouse)) {
                    if (j >= 0 && j <= 9
                        && (*parent)[i][j].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(sf::Color(
                          255,
                          0,
                          0,
                          100));
                    }
                  }
                }
              } else {
                (*parent)[i][j].setAlpha(0);
              }
            }
          }
          break;
        case 2:
          if (event.type == sf::Event::MouseMoved
              || event.type == sf::Event::MouseButtonReleased) {
            if (GameField::diffY <= 32.f * scale.y) {
              if ((cellRect.contains(mouse) ||
                  (j >= 1 && sf::IntRect((*parent)[i][j
                      - 1].cell.getGlobalBounds()).contains(mouse)))) {
                if (event.type == sf::Event::MouseButtonReleased) {
                  if (cellRect.contains(mouse)) {
                    if (parent->ship2.size() < 3) {
                      parent->addShip(i, j, GameField::shipSize);
                    }
                    for (int j_ = 0; j_ <= 1; j_++) {
                      if (j + j_ >= 0 && j + j_ <= 9) {
                        (*parent)[i][j + j_].setAlpha(0);
                      }
                    }
                  }
                } else {
                  if (cellRect.contains(mouse)) {
                    if (j >= 0 && j <= 8
                        && (*parent)[i][j].isAvailable()
                        && (*parent)[i][j + 1].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(sf::Color(
                          255,
                          0,
                          0,
                          100));
                    }
                  } else if (j >= 1 && sf::IntRect((*parent)[i][j
                      - 1].cell.getGlobalBounds()).contains(mouse)) {
                    if (j >= 1 && j <= 9
                        && (*parent)[i][j].isAvailable()
                        && (*parent)[i][j - 1].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(sf::Color(
                          255,
                          0,
                          0,
                          100));
                    }
                  }
                }
              } else {
                (*parent)[i][j].setAlpha(0);
              }
            } else {
              if ((cellRect.contains(mouse) ||
                  (j <= 8 && sf::IntRect((*parent)[i][j
                      + 1].cell.getGlobalBounds()).contains(mouse)))) {
                if (event.type == sf::Event::MouseButtonReleased) {
                  if (cellRect.contains(mouse)) {
                    if (parent->ship2.size() < 3) {
                      parent->addShip(i,
                                      j - 1,
                                      GameField::shipSize);
                    }
                    for (int j_ = -1; j_ <= 0; j_++) {
                      if (j + j_ >= 0 && j + j_ <= 9) {
                        (*parent)[i][j + j_].setAlpha(0);
                      }
                    }
                  }
                } else {
                  if (cellRect.contains(mouse)) {
                    if (j >= 1 && j <= 9
                        && (*parent)[i][j].isAvailable()
                        && (*parent)[i][j - 1].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(sf::Color(
                          255,
                          0,
                          0,
                          100));
                    }
                  } else if (j <= 8 && sf::IntRect((*parent)[i][j
                      + 1].cell.getGlobalBounds()).contains(mouse)) {
                    if (j >= 0 && j <= 8
                        && (*parent)[i][j].isAvailable()
                        && (*parent)[i][j + 1].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(sf::Color(
                          255,
                          0,
                          0,
                          100));
                    }
                  }
                }
              } else {
                (*parent)[i][j].setAlpha(0);
              }
            }
          }
          break;
        case 3:
          if (event.type == sf::Event::MouseMoved
              || event.type == sf::Event::MouseButtonReleased) {
            if (GameField::diffY <= 32.f * scale.y) {
              if ((cellRect.contains(mouse)) ||
                  (j >= 1 && sf::IntRect((*parent)[i][j
                      - 1].cell.getGlobalBounds()).contains(mouse)) ||
                  (j >= 2 && sf::IntRect((*parent)[i][j
                      - 2].cell.getGlobalBounds()).contains(mouse))) {
                if (event.type == sf::Event::MouseButtonReleased) {
                  if (cellRect.contains(mouse)) {
                    if (parent->ship3.size() < 2) {
                      parent->addShip(i, j, GameField::shipSize);
                    }
                    for (int j_ = 0; j_ <= 2; j_++) {
                      if (j + j_ >= 0 && j + j_ <= 9) {
                        (*parent)[i][j + j_].setAlpha(0);
                      }
                    }
                  }
                } else {
                  if (j >= 1 && sf::IntRect((*parent)[i][j
                      - 1].cell.getGlobalBounds()).contains(mouse)) {
                    if (j >= 1 && j <= 8
                        && (*parent)[i][j].isAvailable()
                        && (*parent)[i][j - 1].isAvailable() &&
                        (*parent)[i][j + 1].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(sf::Color(
                          255,
                          0,
                          0,
                          100));
                    }
                  } else if (j >= 2 && sf::IntRect((*parent)[i][j
                      - 2].cell.getGlobalBounds()).contains(mouse)) {
                    if (j >= 2 && j <= 9
                        && (*parent)[i][j].isAvailable()
                        && (*parent)[i][j - 1].isAvailable() &&
                        (*parent)[i][j - 2].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(sf::Color(
                          255,
                          0,
                          0,
                          100));
                    }
                  } else if (cellRect.contains(mouse)) {
                    if (j >= 0 && j <= 7
                        && (*parent)[i][j].isAvailable()
                        && (*parent)[i][j + 2].isAvailable() &&
                        (*parent)[i][j + 1].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(sf::Color(
                          255,
                          0,
                          0,
                          100));
                    }
                  }
                }
              } else {
                (*parent)[i][j].setAlpha(0);
              }
            } else if (GameField::diffY > 32 * scale.y
                && GameField::diffY <= 64 * scale.y) {
              if ((cellRect.contains(mouse)) ||
                  (j >= 1 && sf::IntRect((*parent)[i][j
                      - 1].cell.getGlobalBounds()).contains(mouse)) ||
                  (j <= 8 && sf::IntRect((*parent)[i][j
                      + 1].cell.getGlobalBounds()).contains(mouse))) {
                if (event.type == sf::Event::MouseButtonReleased) {
                  if (cellRect.contains(mouse)) {
                    if (parent->ship3.size() < 2) {
                      parent->addShip(i,
                                      j - 1,
                                      GameField::shipSize);
                    }
                    for (int j_ = -1; j_ <= 1; j_++) {
                      if (j + j_ >= 0 && j + j_ <= 9) {
                        (*parent)[i][j + j_].setAlpha(0);
                      }
                    }
                  }
                } else {
                  if ((cellRect.contains(mouse))) {
                    if (j >= 1 && j <= 8
                        && (*parent)[i][j].isAvailable()
                        && (*parent)[i][j - 1].isAvailable() &&
                        (*parent)[i][j + 1].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(sf::Color(
                          255,
                          0,
                          0,
                          100));
                    }
                  } else if (j >= 1 && sf::IntRect((*parent)[i][j
                      - 1].cell.getGlobalBounds()).contains(mouse)) {
                    if (j >= 2 && j <= 9
                        && (*parent)[i][j].isAvailable()
                        && (*parent)[i][j - 1].isAvailable() &&
                        (*parent)[i][j - 2].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(sf::Color(
                          255,
                          0,
                          0,
                          100));
                    }
                  } else if (j <= 8 && sf::IntRect((*parent)[i][j
                      + 1].cell.getGlobalBounds()).contains(mouse)) {
                    if (j >= 0 && j <= 7
                        && (*parent)[i][j].isAvailable()
                        && (*parent)[i][j + 2].isAvailable() &&
                        (*parent)[i][j + 1].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(sf::Color(
                          255,
                          0,
                          0,
                          100));
                    }
                  }
                }
              } else {
                (*parent)[i][j].setAlpha(0);
              }
            } else {
              if ((cellRect.contains(mouse)) ||
                  (j <= 7 && sf::IntRect((*parent)[i][j
                      + 2].cell.getGlobalBounds()).contains(mouse)) ||
                  (j <= 8 && sf::IntRect((*parent)[i][j
                      + 1].cell.getGlobalBounds()).contains(mouse))) {
                if (event.type == sf::Event::MouseButtonReleased) {
                  if (cellRect.contains(mouse)) {
                    if (parent->ship3.size() < 2) {
                      parent->addShip(i,
                                      j - '\2',
                                      GameField::shipSize);
                    }
                    for (int j_ = -2; j_ <= 0; j_++) {
                      if (j + j_ >= 0 && j + j_ <= 9) {
                        (*parent)[i][j + j_].setAlpha(0);
                      }
                    }
                  }
                } else {
                  if (j <= 8 && sf::IntRect((*parent)[i][j
                      + 1].cell.getGlobalBounds()).contains(mouse)) {
                    if (j >= 1 && j <= 8
                        && (*parent)[i][j].isAvailable()
                        && (*parent)[i][j - 1].isAvailable() &&
                        (*parent)[i][j + 1].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(sf::Color(
                          255,
                          0,
                          0,
                          100));
                    }
                  } else if (cellRect.contains(mouse)) {
                    if (j >= 2 && j <= 9
                        && (*parent)[i][j].isAvailable()
                        && (*parent)[i][j - 1].isAvailable() &&
                        (*parent)[i][j - 2].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(sf::Color(
                          255,
                          0,
                          0,
                          100));
                    }
                  } else if (j <= 7 && sf::IntRect((*parent)[i][j
                      + 2].cell.getGlobalBounds()).contains(mouse)) {
                    if (j >= 0 && j <= 7
                        && (*parent)[i][j].isAvailable()
                        && (*parent)[i][j + 2].isAvailable() &&
                        (*parent)[i][j + 1].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(sf::Color(
                          255,
                          0,
                          0,
                          100));
                    }
                  }
                }
              } else {
                (*parent)[i][j].setAlpha(0);
              }
            }
          }
          break;
        case 4:
          if (event.type == sf::Event::MouseMoved
              || event.type == sf::Event::MouseButtonReleased) {
            if (GameField::diffY <= 32.f * scale.y) {
              if ((cellRect.contains(mouse)) ||
                  (j >= 1 && sf::IntRect((*parent)[i][j
                      - 1].cell.getGlobalBounds()).contains(mouse)) ||
                  (j >= 2 && sf::IntRect((*parent)[i][j
                      - 2].cell.getGlobalBounds()).contains(mouse)) ||
                  (j >= 3 && sf::IntRect((*parent)[i][j
                      - 3].cell.getGlobalBounds()).contains(mouse))) {
                if (event.type == sf::Event::MouseButtonReleased) {
                  if (cellRect.contains(mouse)) {
                    if (parent->ship4.size() < 1) {
                      parent->addShip(i, j, GameField::shipSize);
                    }
                    for (int j_ = 0; j_ <= 3; j_++) {
                      if (j + j_ >= 0 && j + j_ <= 9) {
                        (*parent)[i][j + j_].setAlpha(0);
                      }
                    }
                  }
                } else {
                  if (sf::IntRect(
                      (*parent)[i][j].cell.getGlobalBounds())
                      .contains(mouse)) {
                    if (j >= 0 && j <= 6
                        && (*parent)[i][j + 0].isAvailable()
                        && (*parent)[i][j + 1].isAvailable() &&
                        (*parent)[i][j + 2].isAvailable()
                        && (*parent)[i][j + 3].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(sf::Color(
                          255,
                          0,
                          0,
                          100));
                    }
                  } else if (j >= 1 && sf::IntRect((*parent)[i][j
                      - 1].cell.getGlobalBounds()).contains(mouse)) {
                    if (j >= 1 && j <= 7
                        && (*parent)[i][j - 1].isAvailable()
                        && (*parent)[i][j + 0].isAvailable() &&
                        (*parent)[i][j + 1].isAvailable()
                        && (*parent)[i][j + 2].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(sf::Color(
                          255,
                          0,
                          0,
                          100));
                    }
                  } else if (j >= 2 && sf::IntRect((*parent)[i][j
                      - 2].cell.getGlobalBounds()).contains(mouse)) {
                    if (j >= 2 && j <= 8
                        && (*parent)[i][j - 2].isAvailable()
                        && (*parent)[i][j - 1].isAvailable() &&
                        (*parent)[i][j + 0].isAvailable()
                        && (*parent)[i][j + 1].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(sf::Color(
                          255,
                          0,
                          0,
                          100));
                    }
                  } else if (j >= 3 && sf::IntRect((*parent)[i][j
                      - 3].cell.getGlobalBounds()).contains(mouse)) {
                    if (j >= 3 && j <= 9
                        && (*parent)[i][j - 3].isAvailable()
                        && (*parent)[i][j - 2].isAvailable() &&
                        (*parent)[i][j - 1].isAvailable()
                        && (*parent)[i][j + 0].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(
                          sf::Color(255, 0, 0, 100));
                    }
                  }
                }
              } else {
                (*parent)[i][j].setAlpha(0);
              }
            } else if (GameField::diffY > 32.f * scale.y
                && GameField::diffY <= 64.f * scale.y) {
              if ((cellRect.contains(mouse)) ||
                  (j >= 1 && sf::IntRect(
                      (*parent)[i][j - 1].cell.getGlobalBounds())
                      .contains(mouse)) ||
                  (j >= 2 && sf::IntRect(
                      (*parent)[i][j - 2].cell.getGlobalBounds())
                      .contains(mouse)) ||
                  (j <= 8 && sf::IntRect(
                      (*parent)[i][j + 1].cell.getGlobalBounds())
                      .contains(mouse))) {
                if (event.type == sf::Event::MouseButtonReleased) {
                  if (cellRect.contains(mouse)) {
                    if (parent->ship4.empty()) {
                      parent->addShip(i, j - 1, GameField::shipSize);
                    }
                    for (int j_ = -1; j_ <= 2; j_++) {
                      if (j + j_ >= 0 && j + j_ <= 9) {
                        (*parent)[i][j + j_].setAlpha(0);
                      }
                    }
                  }
                } else {
                  if (sf::IntRect(
                      (*parent)[i][j].cell.getGlobalBounds())
                      .contains(mouse)) {
                    if (j >= 1 && j <= 7
                        && (*parent)[i][j - 1].isAvailable()
                        && (*parent)[i][j + 0].isAvailable()
                        && (*parent)[i][j + 1].isAvailable()
                        && (*parent)[i][j + 2].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(
                          sf::Color(255,
                                    0, 0, 100));
                    }
                  } else if (j >= 1 && sf::IntRect(
                      (*parent)[i][j - 1].cell.getGlobalBounds())
                      .contains(mouse)) {
                    if (j >= 2 && j <= 8
                        && (*parent)[i][j - 2].isAvailable()
                        && (*parent)[i][j - 1].isAvailable()
                        && (*parent)[i][j + 0].isAvailable()
                        && (*parent)[i][j + 1].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(
                          sf::Color(255, 0, 0, 100));
                    }
                  } else if (j >= 2 && sf::IntRect(
                      (*parent)[i][j - 2].cell.getGlobalBounds())
                      .contains(mouse)) {
                    if (j >= 3 && j <= 9
                        && (*parent)[i][j - 3].isAvailable()
                        && (*parent)[i][j - 2].isAvailable()
                        && (*parent)[i][j - 1].isAvailable()
                        && (*parent)[i][j + 0].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(
                          sf::Color(255, 0, 0, 100));
                    }
                  } else if (j <= 8 && sf::IntRect(
                      (*parent)[i][j + 1].cell.getGlobalBounds())
                      .contains(mouse)) {
                    if (j >= 0 && j <= 6
                        && (*parent)[i][j + 0].isAvailable()
                        && (*parent)[i][j + 1].isAvailable()
                        && (*parent)[i][j + 2].isAvailable()
                        && (*parent)[i][j + 3].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(
                          sf::Color(255, 0, 0, 100));
                    }
                  }
                }
              } else {
                (*parent)[i][j].setAlpha(0);
              }
            } else if (GameField::diffY > 64.f * scale.y
                && GameField::diffY <= 96.f * scale.y) {
              if ((cellRect.contains(mouse)) ||
                  (j >= 1 && sf::IntRect(
                      (*parent)[i][j - 1].cell.getGlobalBounds())
                      .contains(mouse)) ||
                  (j <= 7 && sf::IntRect(
                      (*parent)[i][j + 2].cell.getGlobalBounds())
                      .contains(mouse)) ||
                  (j <= 8 && sf::IntRect(
                      (*parent)[i][j + 1].cell.getGlobalBounds())
                      .contains(mouse))) {
                if (event.type == sf::Event::MouseButtonReleased) {
                  if (cellRect.contains(mouse)) {
                    if (parent->ship4.empty()) {
                      parent->addShip(i, j - 2, GameField::shipSize);
                    }
                    for (int j_ = -2; j_ <= 1; j_++) {
                      if (j + j_ >= 0 && j + j_ <= 9) {
                        (*parent)[i][j + j_].setAlpha(0);
                      }
                    }
                  }
                } else {
                  if (sf::IntRect(
                      (*parent)[i][j].cell.getGlobalBounds())
                      .contains(mouse)) {
                    if (j >= 2 && j <= 8
                        && (*parent)[i][j - 2].isAvailable()
                        && (*parent)[i][j - 1].isAvailable()
                        && (*parent)[i][j + 0].isAvailable()
                        && (*parent)[i][j + 1].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(
                          sf::Color(255, 0, 0, 100));
                    }
                  } else if (j >= 1 && sf::IntRect(
                      (*parent)[i][j - 1].cell.getGlobalBounds())
                      .contains(mouse)) {
                    if (j >= 3 && j <= 9
                        && (*parent)[i][j - 3].isAvailable()
                        && (*parent)[i][j - 2].isAvailable()
                        && (*parent)[i][j - 1].isAvailable()
                        && (*parent)[i][j + 0].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(
                          sf::Color(255, 0, 0, 100));
                    }
                  } else if (j <= 7 && sf::IntRect((*parent)[i][j
                      + 2].cell.getGlobalBounds()).contains(mouse)) {
                    if (j >= 0 && j <= 6
                        && (*parent)[i][j + 0].isAvailable()
                        && (*parent)[i][j + 1].isAvailable()
                        && (*parent)[i][j + 2].isAvailable()
                        && (*parent)[i][j + 3].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(
                          sf::Color(255, 0, 0, 100));
                    }
                  } else if (j <= 8 && sf::IntRect(
                      (*parent)[i][j + 1].cell.getGlobalBounds())
                      .contains(mouse)) {
                    if (j >= 1 && j <= 7
                        && (*parent)[i][j - 1].isAvailable()
                        && (*parent)[i][j + 0].isAvailable()
                        && (*parent)[i][j + 1].isAvailable()
                        && (*parent)[i][j + 2].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(
                          sf::Color(255, 0, 0, 100));
                    }
                  }
                }
              } else {
                (*parent)[i][j].setAlpha(0);
              }
            } else {
              if ((cellRect.contains(mouse)) ||
                  (j <= 8 && sf::IntRect(
                      (*parent)[i][j + 1].cell.getGlobalBounds())
                      .contains(mouse)) ||
                  (j <= 7 && sf::IntRect(
                      (*parent)[i][j + 2].cell.getGlobalBounds())
                      .contains(mouse)) ||
                  (j <= 6 && sf::IntRect(
                      (*parent)[i][j + 3].cell.getGlobalBounds())
                      .contains(mouse))) {
                if (event.type == sf::Event::MouseButtonReleased) {
                  if (cellRect.contains(mouse)) {
                    if (parent->ship4.empty()) {
                      parent->addShip(i,
                                      j - 3,
                                      GameField::shipSize);
                    }
                    for (int j_ = -3; j_ <= 0; j_++) {
                      if (j + j_ >= 0 && j + j_ <= 9) {
                        (*parent)[i][j + j_].setAlpha(0);
                      }
                    }
                  }
                } else {
                  if (sf::IntRect(
                      (*parent)[i][j].cell.getGlobalBounds())
                      .contains(mouse)) {
                    if (j >= 3 && j <= 9
                        && (*parent)[i][j - 3].isAvailable()
                        && (*parent)[i][j - 2].isAvailable()
                        && (*parent)[i][j - 1].isAvailable()
                        && (*parent)[i][j + 0].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(
                          sf::Color(255, 0, 0, 100));
                    }
                  } else if (j <= 6 && sf::IntRect(
                      (*parent)[i][j + 3].cell.getGlobalBounds())
                      .contains(mouse)) {
                    if (j >= 0 && j <= 6
                        && (*parent)[i][j + 0].isAvailable()
                        && (*parent)[i][j + 1].isAvailable()
                        && (*parent)[i][j + 2].isAvailable()
                        && (*parent)[i][j + 3].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(
                          sf::Color(255, 0, 0, 100));
                    }
                  } else if (j <= 7 && sf::IntRect(
                      (*parent)[i][j + 2].cell.getGlobalBounds())
                      .contains(mouse)) {
                    if (j >= 1 && j <= 7
                        && (*parent)[i][j - 1].isAvailable()
                        && (*parent)[i][j + 0].isAvailable()
                        && (*parent)[i][j + 1].isAvailable()
                        && (*parent)[i][j + 2].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(
                          sf::Color(255, 0, 0, 100));
                    }
                  } else if (j <= 8 && sf::IntRect(
                      (*parent)[i][j + 1].cell.getGlobalBounds())
                      .contains(mouse)) {
                    if (j >= 2 && j <= 8
                        && (*parent)[i][j - 2].isAvailable()
                        && (*parent)[i][j - 1].isAvailable()
                        && (*parent)[i][j + 0].isAvailable()
                        && (*parent)[i][j + 1].isAvailable()) {
                      (*parent)[i][j].setAlpha(100);
                    } else {
                      (*parent)[i][j].cell.setFillColor(
                          sf::Color(255, 0, 0, 100));
                    }
                  }
                }
              } else {
                (*parent)[i][j].setAlpha(0);
              }
            }
          }
          break;
        default: break;
      }
      break;
    case INACTIVE:
    case ENEMY_INACTIVE: break;
  }
}

void GameFieldCell::draw() const {
  window->draw(cell);
}

void GameFieldCell::setAlpha(unsigned char alpha) {
  cell.setFillColor(sf::Color(255, 255, 255, alpha));
}

bool GameFieldCell::isAvailable() const {
  return availability;
}

void GameFieldCell::rmAvailability() {
  availability = false;
}

void GameFieldCell::addAvailability() {
  availability = true;
}

void GameFieldCell::shoot() {
  rmAvailability();
  if (underShip) {
    cell.setFillColor(sf::Color(255, 0, 0, 100));
  } else {
    cell.setFillColor(sf::Color(50, 50, 50, 100));
  }
}

void GameFieldCell::setUnderShip(bool under) {
  underShip = under;
}

bool GameFieldCell::isUnderShip() const {
  return underShip;
}
