#include "game_field.h"
#include "battle_ship_game.h"


char GameField::shipSize = 0;
float GameField::diffY = 0.f;


DraggableAndDroppableShips::DraggableAndDroppableShips(const sf::Vector2<float> &scale_, const sf::VideoMode &screen,
                                                       std::shared_ptr<sf::RenderWindow> window_): ScreenObject(window_), scale(scale_),
                                                                                                   dragged({false}) {
    float nextYPosition = 0;
    for (char i = 1; i <= 4; i++) {
        tShip[i].loadFromFile(Config::instance().resources + "Ship" + std::to_string(i) + "_a.png");
        sShip[i].setTexture(tShip[i]);
        sShip[i].setScale(scale);
        startPos[i] = sf::Vector2<float>(
                static_cast<float>(screen.width) * 0.5f - sShip[i].getGlobalBounds().width / 2.0f - 50.f,
                static_cast<float>(screen.height) * 0.15f + nextYPosition);
        nextYPosition += sShip[i].getGlobalBounds().height;
        sShip[i].setPosition(startPos[i]);
    }
}

void DraggableAndDroppableShips::draw() const {
    for (int i = 1; i <= 4; i++) {
        window->draw(sShip[i]);
    }
}

void DraggableAndDroppableShips::eventCheck(sf::Event& event) {
    auto mouse = static_cast<sf::Vector2<float>>(sf::Mouse::getPosition(*window));
    for (char i = 1; i <= 4; i++) {
        auto rect = sShip[i].getGlobalBounds();
        rect.height -= 32 * scale.y;
        rect.width -= 32 * scale.x;
        rect.top += 16 * scale.y;
        rect.left += 16 * scale.x;
        if (sf::IntRect(rect).contains(static_cast<sf::Vector2<int>>(mouse))) {
            if (event.type == sf::Event::MouseButtonPressed) {
                dragged[i] = true;
                GameField::shipSize = i;
                GameField::diffY = mouse.y - rect.top;
                tempPos[i] = mouse - sShip[i].getPosition();
            }
        }
        if (event.type == sf::Event::MouseButtonReleased) {
            sShip[i].setPosition(startPos[i]);
            dragged[i] = false;
            GameField::shipSize = 0;
        }
        if (event.type == sf::Event::MouseMoved) {
            if (dragged[i]) {
                sShip[i].setPosition(mouse - tempPos[i]);
            }
        }
    }
}


GameField::GameField(sf::Vector2<float> position_, sf::Vector2<float> scale_, GameFieldState state_,
                     std::shared_ptr<sf::RenderWindow> window_, std::function<void()> changeSide_):
        ScreenObject(window_), field("Field1.bmp", position_, scale_, window_), changeSide(std::move(changeSide_)),
        border("fieldLetters.png", sf::Vector2<float>(position_.x - 32 * scale_.x, position_.y - 32 * scale_.y), scale_, window_),
        cells(10), state(state_), scale(scale_), position(position_) {
    int row = 0, col = 0;
    const int rectSize = 32;
    for (auto &item: cells) {
        col = 0;
        for (int j = 0; j < 10; j++) {
            item.emplace_back(scale, position, window_);
            item[j].setPosition({position.x + static_cast<float>(rectSize * row + row) * scale.x,
                                 position.y + static_cast<float>(rectSize * col + col) * scale.y});
            col++;
        }
        row++;
    }
}

template <char N>
void GameField::updateAvailability(Ship<N> &ship) {
    for (auto &s: ship1) {
        if (s.getCoords() != ship.getCoords()) {
            s.updateAvailability(cells);
        }
    }
    for (auto &s: ship2) {
        if (s.getCoords() != ship.getCoords()) {
            s.updateAvailability(cells);
        }
    }
    for (auto &s: ship3) {
        if (s.getCoords() != ship.getCoords()) {
            s.updateAvailability(cells);
        }
    }
    for (auto &s: ship4) {
        if (s.getCoords() != ship.getCoords()) {
            s.updateAvailability(cells);
        }
    }
}

void GameField::clearAvailability(bool onlyAvailability) {
    for (auto &i: cells) {
        for (auto &c: i) {
            c.addAvailability();
            if (!onlyAvailability) {
                c.setUnderShip(false);
            }
        }
    }
}

void GameField::eventCheck(sf::Event &event) {
    switch (state) {
        case INACTIVE:
            break;
        case PLACEMENT:
            clearAvailability();

            for (auto &s: ship1) {
                if (s.eventCheck(event, cells, [this](Ship<1> &ship) { updateAvailability(ship); },
                                 [this](std::pair<char, char> coords, char shipSize) { removeShip(coords, shipSize); }) == REMOVED) {
                    return;
                }
                s.updateAvailability(cells);
            }

            for (auto &s: ship2) {
                if (s.eventCheck(event, cells, [this](Ship<2> &ship) { updateAvailability(ship); },
                                 [this](std::pair<char, char> coords, char shipSize) { removeShip(coords, shipSize); }) == REMOVED) {
                    return;
                }
                s.updateAvailability(cells);
            }

            for (auto &s: ship3) {
                if (s.eventCheck(event, cells, [this](Ship<3> &ship) { updateAvailability(ship); },
                                 [this](std::pair<char, char> coords, char shipSize) { removeShip(coords, shipSize); }) == REMOVED) {
                    return;
                }
                s.updateAvailability(cells);
            }

            for (auto &s: ship4) {
                if (s.eventCheck(event, cells, [this](Ship<4> &ship) { updateAvailability(ship); },
                                 [this](std::pair<char, char> coords, char shipSize) { removeShip(coords, shipSize); }) == REMOVED) {
                    return;
                }
                s.updateAvailability(cells);
            }

            for (char i = 0; i < cells.size(); i++) {
                for (char j = 0; j < cells[i].size(); j++) {
                    cells[i][j].eventCheck(event, state, *this, i, j, scale);
                }
            }
            break;
        case GAME:
            for (char i = 0; i < cells.size(); i++) {
                for (char j = 0; j < cells[i].size(); j++) {
                    cells[i][j].eventCheck(event, state, *this, i, j, scale);
                }
            }
            break;
    }
}

void GameField::draw() const {
    field.draw();
    border.draw();
    for (auto &i: cells) {
        for (auto &c: i) {
            c.draw();
        }
    }
    switch (state) {
        case INACTIVE:
        case GAME:
        case PLACEMENT:
            drawShips(ship1);
            drawShips(ship2);
            drawShips(ship3);
            drawShips(ship4);
            break;
    }
}

template<char N>
void GameField::drawShips(const std::list<Ship<N>> &ships) const {
    switch (state) {
        case GAME:
            for (auto &ship: ships) {
                if (!ship.isAlive()) {
                    ship.draw();
                }
            }
            //TODO break;
        case INACTIVE:
        case PLACEMENT:
            for (auto &ship: ships) {
                ship.draw();
            }
            break;
    }
}

bool GameField::addShip(char i, char j, char shipType) {
    if (i >= 0 && i <= 9 && j >= 0 && j <= 9 && cells[i][j].isAvailable()) {
        auto newPosition = sf::Vector2<float>(position.x + (33.f * static_cast<float>(i) + 16.f) * scale.x,
                                              position.y + (33.f * static_cast<float>(j) + 16.f) * scale.y);
        switch (shipType) {
            case 1:
                if (cells[i][j].isAvailable()) {
                    ship1.emplace_back(scale, std::pair{i, j}, newPosition, window);
                } else {
                    return false;
                }
                break;
            case 2:
                if (j <= 8 && cells[i][j].isAvailable() && cells[i][j + 1].isAvailable()) {
                    ship2.emplace_back(scale, std::pair{i, j}, newPosition, window);
                } else {
                    return false;
                }
                break;
            case 3:
                if (j <= 7 && cells[i][j].isAvailable() && cells[i][j + 1].isAvailable() && cells[i][j + 2].isAvailable()) {
                    ship3.emplace_back(scale, std::pair{i, j}, newPosition, window);
                } else {
                    return false;
                }
                break;
            case 4:
                if (j <= 6 && cells[i][j].isAvailable() && cells[i][j + 1].isAvailable() && cells[i][j + 2].isAvailable() &&
                    cells[i][j + 3].isAvailable()) {
                    ship4.emplace_back(scale, std::pair{i, j}, newPosition, window);
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

void GameField::setState(GameFieldState newState) {
    state = newState;
}

void GameField::removeShip(std::pair<char, char> coords, char shipType) {
    switch (shipType) {
        case 1:
            for (auto it = ship1.begin(); it != ship1.end(); it++) {
                if (it->getCoords().first == coords.first && it->getCoords().second == coords.second) {
                    ship1.erase(it);
                    return;
                }
            }
            break;
        case 2:
            for (auto it = ship2.begin(); it != ship2.end(); it++) {
                if (it->getCoords().first == coords.first && it->getCoords().second == coords.second) {
                    ship2.erase(it);
                    return;
                }
            }
            break;
        case 3:
            for (auto it = ship3.begin(); it != ship3.end(); it++) {
                if (it->getCoords().first == coords.first && it->getCoords().second == coords.second) {
                    ship3.erase(it);
                    return;
                }
            }
            break;
        case 4:
            for (auto it = ship4.begin(); it != ship4.end(); it++) {
                if (it->getCoords().first == coords.first && it->getCoords().second == coords.second) {
                    ship4.erase(it);
                    return;
                }
            }
            break;
        default:
            break;
    }
}

void GameField::clearShips() {
    ship1.clear();
    ship2.clear();
    ship3.clear();
    ship4.clear();
}

void GameField::placeShipsRand() {
    clearShips();
    clearAvailability();
    std::mt19937 engine;
    engine.seed(std::time(nullptr));
    for (char size = 1; size <= 4; size++) {
        for (char i = 0; i <= 4 - size; i++) {
            if (!addShip(static_cast<char>(engine() % 10), static_cast<char>(engine() % 10), size)) {
                for (char i_ = 0; i_ < 10; i_++) {
                    for (char j_ = 0; j_ < 10; j_++) {
                        if (addShip(i_, j_, size)) {
                            i_ = 10;
                            j_ = 10;
                        }
                    }
                }
            }
            switch (size) {
                case 1:
                    (--(ship1.end()))->updateAvailability(cells);
                    break;
                case 2:
                    (--(ship2.end()))->updateAvailability(cells);
                    break;
                case 3:
                    (--(ship3.end()))->updateAvailability(cells);
                    break;
                case 4:
                    (--(ship4.end()))->updateAvailability(cells);
                    break;
            }
        }
    }
}

std::vector<GameFieldCell>& GameField::operator[](size_t i) {
    if (i <= cells.size()) {
        return cells[i];
    }
    throw std::runtime_error("Invalid index in GameField!\n");
}

void GameField::selfMove(std::pair<unsigned char, unsigned char> move) {
    sf::sleep(sf::seconds(0.1f));
    if (move == std::pair<unsigned char, unsigned char>{100, 100}) {
        char i = 0, j = 0;
        for (auto &v: cells) {
            for (auto &c: v) {
                if (c.isAvailable()) {
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
    } else {
        cells[move.first][move.second].shoot();
        if (cells[move.first][move.second].underShip) {
            findShip(move);
        }
        changeSide();
    }
}

void GameField::clearColors() {
    for (auto& vec: cells){
        for (auto& c: vec){
            c.setAlpha(0);
        }
    }
}

template<char N>
void GameField::shoot(std::pair<char, char> coords, Ship<N> &ship) {
    for (int i = 0; i < N; i++) {
        switch (ship.getDirection()) {
            case UP:
                if (coords.first == ship.getCoords().first &&
                    coords.second == ship.getCoords().second + i) {
                    ship.shoot();
                    if (!ship.isAlive()){
                        for (int i_ = -1; i_ <= 1; i_++){
                            for (int j_ = -1; j_ <= N; j_++) {
                                if (Ship<N>::coordsValid(std::pair(coords.first + i_, coords.second + j_ - i))) {
                                    cells[coords.first + i_][coords.second + j_ - i].shoot();
                                }
                            }
                        }
                    }
                    return;
                }
                break;
            case RIGHT:
                if (coords.first == ship.getCoords().first - i &&
                    coords.second == ship.getCoords().second) {
                    ship.shoot();
                    if (!ship.isAlive()){
                        for (int i_ = -1; i_ <= 1; i_++){
                            for (int j_ = -1; j_ <= N; j_++) {
                                if (Ship<N>::coordsValid(std::pair(coords.first - j_ + i, coords.second + i_))) {
                                    cells[coords.first - j_ + i][coords.second + i_].shoot();
                                }
                            }
                        }
                    }
                    return;
                }
                break;
            case DOWN:
                if (coords.first == ship.getCoords().first &&
                    coords.second == ship.getCoords().second - i) {
                    ship.shoot();
                    if (!ship.isAlive()){
                        for (int i_ = -1; i_ <= 1; i_++){
                            for (int j_ = -1; j_ <= N; j_++) {
                                if (Ship<N>::coordsValid(std::pair(coords.first + i_, coords.second - j_ + i))) {
                                    cells[coords.first + i_][coords.second - j_ + i].shoot();
                                }
                            }
                        }
                    }
                    return;
                }
                break;
            case LEFT:
                if (coords.first == ship.getCoords().first + i &&
                    coords.second == ship.getCoords().second) {
                    ship.shoot();
                    if (!ship.isAlive()){
                        for (int i_ = -1; i_ <= 1; i_++){
                            for (int j_ = -1; j_ <= N; j_++) {
                                if (Ship<N>::coordsValid(std::pair(coords.first + j_ - i, coords.second + i_))) {
                                    cells[coords.first + j_ - i][coords.second + i_].shoot();
                                }
                            }
                        }
                    }
                    return;
                }
                break;
        }
    }
}

void GameField::findShip(std::pair<char, char> coords) {
    for (auto &s: ship1) {
        shoot(coords, s);
    }
    for (auto &s: ship2) {
        shoot(coords, s);
    }
    for (auto &s: ship3) {
        shoot(coords, s);
    }
    for (auto &s: ship4) {
        shoot(coords, s);
    }
}

unsigned char GameField::getAliveShips() const {
    unsigned char count = 0;
    for (auto& s:ship1) {
        count += (s.isAlive() ? 1 : 0);
    }
    for (auto& s:ship2) {
        count += (s.isAlive() ? 1 : 0);
    }
    for (auto& s:ship3) {
        count += (s.isAlive() ? 1 : 0);
    }
    for (auto& s:ship4) {
        count += (s.isAlive() ? 1 : 0);
    }
    return count;
}

std::vector<sf::Uint16> GameField::serializedField() {
    std::vector<sf::Uint16> result;
    for (auto &s: ship1) {
        result.emplace_back((s.getCoords().first * 64) | (s.getCoords().second * 4) | s.getDirection());
    }
    for (auto &s: ship2) {
        result.emplace_back((s.getCoords().first * 64) | (s.getCoords().second * 4) | s.getDirection());
    }
    for (auto &s: ship3) {
        result.emplace_back((s.getCoords().first * 64) | (s.getCoords().second * 4) | s.getDirection());
    }
    for (auto &s: ship4) {
        result.emplace_back((s.getCoords().first * 64) | (s.getCoords().second * 4) | s.getDirection());
    }
    return std::move(result);
}
