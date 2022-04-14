#include "battle_ship_game.h"

#include <iostream>

const float BattleShipGame::WIDTH = 1920;
const float BattleShipGame::HEIGHT = 1080;

char GameField::shipSize = 0;
float GameField::diffY = 0.f;

template <char N>
std::array<sf::Texture, 5> Ship<N>::alive;

template <char N>
std::array<sf::Texture, 5> Ship<N>::injured;

template <char N>
std::array<sf::Texture, 5> Ship<N>::destroyed;


ShipDirection& operator++(ShipDirection& other){
    switch (other) {
        case UP:
            other = RIGHT;
            break;
        case RIGHT:
            other = DOWN;
            break;
        case DOWN:
            other = LEFT;
            break;
        case LEFT:
            other = UP;
            break;
    }
    return other;
}

const ShipDirection operator++(ShipDirection& other, int){
    ShipDirection rVal = other;
    ++other;
    return rVal;
}


BattleShipGame::BattleShipGame() : server(std::make_unique<sf::TcpSocket>()), screen(sf::VideoMode::getDesktopMode()), status(IN_SP_MENU),
                                   window(std::make_shared<sf::RenderWindow>(sf::VideoMode::getDesktopMode(), "Battleship", sf::Style::Fullscreen)),
                                   screenScale(static_cast<float>(screen.width) / WIDTH, static_cast<float>(screen.height) / HEIGHT) {
    // Connection to server
    if (server->connect(IP_ADDR, PORT) != sf::Socket::Done) {
        throw std::runtime_error("Connection error!\n");
    }

    // Set up framerate limit
    window->setFramerateLimit(FRAMERATE);

    // Cursor init
    cursorTex.loadFromFile(std::string(RESOURCES_PATH) + "cursor.png");
    cursor.setTexture(cursorTex);
    cursor.setScale(2, 2);
    window->setMouseCursorVisible(false);

    // Load all textures
    loadTextures();
}

void BattleShipGame::loadTextures() {
    Ship<1>::loadTextures();
    Ship<2>::loadTextures();
    Ship<3>::loadTextures();
    Ship<4>::loadTextures();
    auto beige = sf::Color(213, 190, 164);
    buttons["exit"] = std::make_unique<Button>(float(screen.width) * 0.05f, float(screen.height) * 0.85f, screenScale,
                                               nullptr, window, "exit", 40, beige);

    buttons["login"] = std::make_unique<Button>(float(screen.width) * 0.22f, float(screen.height) * 0.6f, screenScale * 0.5f,
                                                [this] { loginFunc(); }, window, "login", 40, sf::Color(213, 190, 164));

    entries["login"] = std::make_unique<Entry>(sf::Vector2{float(screen.width) * 0.2f, float(screen.height) * 0.4f}, 12,
                                               window, 24);

    entries["password"] = std::make_unique<Entry>(sf::Vector2{float(screen.width) * 0.2f, float(screen.height) * 0.5f},
                                                  12, window, 24);

    buttons["single"] = std::make_unique<Button>(static_cast<float>(screen.width) * 0.05f, static_cast<float>(screen.height) * 0.15f,
                                                 screenScale, [this] { singlePlayerFunc(); }, window, "singleplayer", 25, beige);
    buttons["multi"] = std::make_unique<Button>(static_cast<float>(screen.width) * 0.05f, static_cast<float>(screen.height) * 0.35f,
                                                screenScale, [this] { multiPlayerFunc(); }, window, "multiplayer", 25, beige);
    pictures["background"] = std::make_unique<Picture>("mainMenu.jpg",
                                                       sf::Vector2<float>{0, 0}, screenScale, window);
    pictures["battleShipText"] = std::make_unique<Picture>("battleship.png",
                                                           sf::Vector2<float>(float(screen.width) * 0.35f,
                                                                              float(screen.height) * 0.85f),
                                                           screenScale, window);

    titles["login"] = std::make_unique<Title>(std::string("Login:"),
                                              sf::Vector2<float>{static_cast<float>(screen.width) * 0.2f + 10.0f,
                                                                 static_cast<float>(screen.height) * 0.4f - 30.0f},
                                              window);
    titles["password"] = std::make_unique<Title>(std::string("Password:"),
                                                 sf::Vector2<float>{static_cast<float>(screen.width) * 0.2f + 10.0f,
                                                                    static_cast<float>(screen.height) * 0.5f - 30.0f},
                                                 window);
    pictures["myField"] = std::make_unique<Picture>("Field1.bmp",
                                                    sf::Vector2<float>(static_cast<float>(screen.width) * 0.1f,
                                                                       static_cast<float>(screen.height) * 0.25f),
                                                    sf::Vector2<float>{screenScale.x * 1.75f, screenScale.y * 1.75f},
                                                    window);
    buttons["mainMenu"] = std::make_unique<Button>(float(screen.width) * 0.05f, float(screen.height) * 0.85f, screenScale,
                                                   [this] { mainMenu(); }, window, "back", 40, beige);
}

void BattleShipGame::mainLoop() {
    sf::Event event{};

    DraggableAndDroppableShips ships(screenScale * 1.75f, screen, window);

    GameField myField({100, 100}, {screenScale.x * 1.75f, screenScale.y * 1.75f}, PLACEMENT, window);

    while (window->isOpen()) {
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window->close();
            switch (status) {
                case LOGIN:
                    entries["login"]->eventCheck(event);
                    entries["password"]->eventCheck(event);
                    buttons["login"]->eventCheck(event);
                    buttons["exit"]->eventCheck(event);
                    break;
                case MAIN_MENU:
                    buttons["single"]->eventCheck(event);
                    buttons["multi"]->eventCheck(event);
                    buttons["exit"]->eventCheck(event);
                    break;
                case IN_SP_MENU:
                    myField.eventCheck(event);
                    buttons["mainMenu"]->eventCheck(event);
                    ships.eventCheck(event);
                    break;
                default:
                    std::cerr << "Wrong status\n";
            }
        }
        pictures["background"]->draw();
        pictures["battleShipText"]->draw();

        switch (status) {
            case LOGIN:
                entries["login"]->draw();
                titles["login"]->draw();
                entries["password"]->draw();
                titles["password"]->draw();
                buttons["login"]->draw();
                buttons["exit"]->draw();
                break;
            case MAIN_MENU:
                buttons["single"]->draw();
                buttons["multi"]->draw();
                buttons["exit"]->draw();
                break;
            case IN_SP_MENU:
                buttons["mainMenu"]->draw();
                myField.draw();
                ships.draw();
                break;
            default:
                std::cerr << "Wrong status\n";
        }
        cursor.setPosition(static_cast<float>(sf::Mouse::getPosition().x), static_cast<float>(sf::Mouse::getPosition().y));
        window->draw(cursor);
        window->display();
    }
}

void BattleShipGame::loginFunc() {
    auto login = entries["login"]->getStr();
    auto password = entries["password"]->getStr();
    sf::Packet packet;
    packet << login << password;
    server->send(packet);
    packet.clear();
    bool authDone;
    server->receive(packet);
    packet >> authDone;
    if (authDone) {
        mainMenu();
    }
}

void BattleShipGame::mainMenu(){
    status = MAIN_MENU;
}

void BattleShipGame::singlePlayerFunc() {
    status = IN_SP_MENU;
}

void BattleShipGame::multiPlayerFunc() {

}


DraggableAndDroppableShips::DraggableAndDroppableShips(const sf::Vector2<float> &scale_, const sf::VideoMode &screen,
                                                       std::shared_ptr<sf::RenderWindow> window_): ScreenObject(window_), scale(scale_) {
    float nextYPosition = 0;
    for (char i = 1; i <= 4; i++) {
        tShip[i].loadFromFile(std::string(RESOURCES_PATH) + "Ship" + std::to_string(i) + "_a.png");
        sShip[i].setTexture(tShip[i]);
        sShip[i].setScale(scale);
        startPos[i] = sf::Vector2<float>(
                static_cast<float>(screen.width) * 0.5f - sShip[i].getGlobalBounds().width / 2.0f,
                static_cast<float>(screen.height) * 0.1f + nextYPosition);
        nextYPosition += sShip[i].getGlobalBounds().height;
        sShip[i].setPosition(startPos[i]);
        dragged[i] = false;
    }
}

void DraggableAndDroppableShips::draw() const {
    for (int i = 1; i <= 4; i++) {
        window->draw(sShip[i]);
    }
}

void DraggableAndDroppableShips::eventCheck(sf::Event& event) {
    for (char i = 1; i <= 4; i++) {
        auto rect = sShip[i].getGlobalBounds();
        rect.height -= 32 * scale.y;
        rect.width -= 32 * scale.x;
        rect.top += 16 * scale.y;
        rect.left += 16 * scale.x;
        if (sf::IntRect(rect).contains(sf::Mouse::getPosition(*window))) {
            if (event.type == sf::Event::MouseButtonPressed) {
                dragged[i] = true;
                GameField::shipSize = i;
                GameField::diffY = static_cast<float>(sf::Mouse::getPosition(*window).y) - rect.top;
                tempPos[i] = static_cast<sf::Vector2<float>>(sf::Mouse::getPosition(*window)) - sShip[i].getPosition();
            }
        }
        if (event.type == sf::Event::MouseButtonReleased) {
            sShip[i].setPosition(startPos[i]);
            dragged[i] = false;
            GameField::shipSize = 0;
        }
        if (event.type == sf::Event::MouseMoved) {
            if (dragged[i]) {
                sShip[i].setPosition(static_cast<sf::Vector2<float>>(sf::Mouse::getPosition(*window)) - tempPos[i]);
            }
        }
    }
}


GameFieldCell::GameFieldCell(sf::Vector2<float> scale, sf::Vector2<float> position, std::shared_ptr<sf::RenderWindow> window_):
        ScreenObject(window_), availability(true) {
    cell.setSize({32, 32});
    cell.setScale(scale);
    cell.setFillColor(sf::Color(255, 255, 255, 0));
    cell.setPosition(position);
}

void GameFieldCell::setPosition(sf::Vector2<float> newPosition) {
    cell.setPosition(newPosition);
}

void GameFieldCell::eventCheck(sf::Event& event, GameFieldState state, GameField& parent,
                               char i, char j, sf::Vector2<float> scale) {
    auto mouse = sf::Mouse::getPosition(*window);
    switch (state) {
        case GAME:
            if (sf::IntRect(cell.getGlobalBounds()).contains(mouse)) {
                parent[i][j].setAlpha(100);
            } else {
                parent[i][j].setAlpha(0);
            }
            break;
        case PLACEMENT:
            switch (GameField::shipSize) {
                case 1:
                    if (event.type == sf::Event::MouseMoved || event.type == sf::Event::MouseButtonReleased) {
                        if (GameField::diffY <= 32.f * scale.y) {
                            if (sf::IntRect(cell.getGlobalBounds()).contains(mouse)) {
                                if (event.type == sf::Event::MouseButtonReleased) {
                                    if (sf::IntRect(cell.getGlobalBounds()).contains(mouse)) {
                                        if (parent.addShip(i, j, GameField::shipSize)) {

                                        }
                                        for (int j_ = 0; j_ <= 0; j_++) {
                                            if (j + j_ >= 0 && j + j_ <= 9) {
                                                parent[i][j + j_].setAlpha(0);
                                            }
                                        }
                                    }
                                } else {
                                    if (sf::IntRect(cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 0 && j <= 9 && parent[i][j].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    }
                                }
                            } else {
                                parent[i][j].setAlpha(0);
                            }
                        }
                    }
                    break;
                case 2:
                    if (event.type == sf::Event::MouseMoved || event.type == sf::Event::MouseButtonReleased) {
                        if (GameField::diffY <= 32.f * scale.y) {
                            if ((sf::IntRect(cell.getGlobalBounds()).contains(mouse) ||
                                 (j >= 1 && sf::IntRect(parent[i][j - 1].cell.getGlobalBounds()).contains(mouse)))) {
                                if (event.type == sf::Event::MouseButtonReleased) {
                                    if (sf::IntRect(cell.getGlobalBounds()).contains(mouse)) {
                                        if (parent.addShip(i, j, GameField::shipSize)) {

                                        }
                                        for (int j_ = 0; j_ <= 1; j_++) {
                                            if (j + j_ >= 0 && j + j_ <= 9) {
                                                parent[i][j + j_].setAlpha(0);
                                            }
                                        }
                                    }
                                } else {
                                    if (sf::IntRect(cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 0 && j <= 8 && parent[i][j].isAvailable() && parent[i][j + 1].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    } else if (j >= 1 && sf::IntRect(parent[i][j - 1].cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 1 && j <= 9 && parent[i][j].isAvailable() && parent[i][j - 1].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    }
                                }
                            } else {
                                parent[i][j].setAlpha(0);
                            }
                        } else {
                            if ((sf::IntRect(cell.getGlobalBounds()).contains(mouse) ||
                                 (j <= 8 && sf::IntRect(parent[i][j + 1].cell.getGlobalBounds()).contains(mouse)))) {
                                if (event.type == sf::Event::MouseButtonReleased) {
                                    if (sf::IntRect(cell.getGlobalBounds()).contains(mouse)) {
                                        if (parent.addShip(i, j - 1, GameField::shipSize)) {

                                        }
                                        for (int j_ = -1; j_ <= 0; j_++) {
                                            if (j + j_ >= 0 && j + j_ <= 9) {
                                                parent[i][j + j_].setAlpha(0);
                                            }
                                        }
                                    }
                                } else {
                                    if (sf::IntRect(cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 1 && j <= 9 && parent[i][j].isAvailable() && parent[i][j - 1].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    } else if (j <= 8 && sf::IntRect(parent[i][j + 1].cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 0 && j <= 8 && parent[i][j].isAvailable() && parent[i][j + 1].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    }
                                }
                            } else {
                                parent[i][j].setAlpha(0);
                            }
                        }
                    }
                    break;
                case 3:
                    if (event.type == sf::Event::MouseMoved || event.type == sf::Event::MouseButtonReleased) {
                        if (GameField::diffY <= 32.f * scale.y) {
                            if ((sf::IntRect(cell.getGlobalBounds()).contains(mouse)) ||
                                (j >= 1 && sf::IntRect(parent[i][j - 1].cell.getGlobalBounds()).contains(mouse)) ||
                                (j >= 2 && sf::IntRect(parent[i][j - 2].cell.getGlobalBounds()).contains(mouse))) {
                                if (event.type == sf::Event::MouseButtonReleased) {
                                    if (sf::IntRect(cell.getGlobalBounds()).contains(mouse)) {
                                        if (parent.addShip(i, j, GameField::shipSize)) {

                                        }
                                        for (int j_ = 0; j_ <= 2; j_++) {
                                            if (j + j_ >= 0 && j + j_ <= 9) {
                                                parent[i][j + j_].setAlpha(0);
                                            }
                                        }
                                    }
                                } else {
                                    if (j >= 1 && sf::IntRect(parent[i][j - 1].cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 1 && j <= 8 && parent[i][j].isAvailable() && parent[i][j - 1].isAvailable() &&
                                            parent[i][j + 1].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    } else if (j >= 2 && sf::IntRect(parent[i][j - 2].cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 2 && j <= 9 && parent[i][j].isAvailable() && parent[i][j - 1].isAvailable() &&
                                            parent[i][j - 2].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    } else if (sf::IntRect(cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 0 && j <= 7 && parent[i][j].isAvailable() && parent[i][j + 2].isAvailable() &&
                                            parent[i][j + 1].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    }
                                }
                            } else {
                                parent[i][j].setAlpha(0);
                            }
                        } else if (GameField::diffY > 32 * scale.y && GameField::diffY <= 64 * scale.y) {
                            if ((sf::IntRect(cell.getGlobalBounds()).contains(mouse)) ||
                                (j >= 1 && sf::IntRect(parent[i][j - 1].cell.getGlobalBounds()).contains(mouse)) ||
                                (j <= 8 && sf::IntRect(parent[i][j + 1].cell.getGlobalBounds()).contains(mouse))) {
                                if (event.type == sf::Event::MouseButtonReleased) {
                                    if (sf::IntRect(cell.getGlobalBounds()).contains(mouse)) {
                                        if (parent.addShip(i, j - 1, GameField::shipSize)) {

                                        }
                                        for (int j_ = -1; j_ <= 1; j_++) {
                                            if (j + j_ >= 0 && j + j_ <= 9) {
                                                parent[i][j + j_].setAlpha(0);
                                            }
                                        }
                                    }
                                } else {
                                    if ((sf::IntRect(cell.getGlobalBounds()).contains(mouse))) {
                                        if (j >= 1 && j <= 8 && parent[i][j].isAvailable() && parent[i][j - 1].isAvailable() &&
                                            parent[i][j + 1].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    } else if (j >= 1 && sf::IntRect(parent[i][j - 1].cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 2 && j <= 9 && parent[i][j].isAvailable() && parent[i][j - 1].isAvailable() &&
                                            parent[i][j - 2].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    } else if (j <= 8 && sf::IntRect(parent[i][j + 1].cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 0 && j <= 7 && parent[i][j].isAvailable() && parent[i][j + 2].isAvailable() &&
                                            parent[i][j + 1].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    }
                                }
                            } else {
                                parent[i][j].setAlpha(0);
                            }
                        } else {
                            if ((sf::IntRect(cell.getGlobalBounds()).contains(mouse)) ||
                                (j <= 7 && sf::IntRect(parent[i][j + 2].cell.getGlobalBounds()).contains(mouse)) ||
                                (j <= 8 && sf::IntRect(parent[i][j + 1].cell.getGlobalBounds()).contains(mouse))) {
                                if (event.type == sf::Event::MouseButtonReleased) {
                                    if (sf::IntRect(cell.getGlobalBounds()).contains(mouse)) {
                                        if (parent.addShip(i, j - 2, GameField::shipSize)) {

                                        }
                                        for (int j_ = -2; j_ <= 0; j_++) {
                                            if (j + j_ >= 0 && j + j_ <= 9) {
                                                parent[i][j + j_].setAlpha(0);
                                            }
                                        }
                                    }
                                } else {
                                    if (j <= 8 && sf::IntRect(parent[i][j + 1].cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 1 && j <= 8 && parent[i][j].isAvailable() && parent[i][j - 1].isAvailable() &&
                                            parent[i][j + 1].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    } else if (sf::IntRect(cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 2 && j <= 9 && parent[i][j].isAvailable() && parent[i][j - 1].isAvailable() &&
                                            parent[i][j - 2].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    } else if (j <= 7 && sf::IntRect(parent[i][j + 2].cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 0 && j <= 7 && parent[i][j].isAvailable() && parent[i][j + 2].isAvailable() &&
                                            parent[i][j + 1].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    }
                                }
                            } else {
                                parent[i][j].setAlpha(0);
                            }
                        }
                    }
                    break;
                case 4:
                    if (event.type == sf::Event::MouseMoved || event.type == sf::Event::MouseButtonReleased) {
                        if (GameField::diffY <= 32.f * scale.y) {
                            if ((sf::IntRect(cell.getGlobalBounds()).contains(mouse)) ||
                                sf::IntRect(parent[i][j - 1].cell.getGlobalBounds()).contains(mouse) ||
                                sf::IntRect(parent[i][j - 2].cell.getGlobalBounds()).contains(mouse) ||
                                sf::IntRect(parent[i][j - 3].cell.getGlobalBounds()).contains(mouse)) {
                                if (event.type == sf::Event::MouseButtonReleased) {
                                    if (sf::IntRect(cell.getGlobalBounds()).contains(mouse)) {
                                        if (parent.addShip(i, j, GameField::shipSize)) {

                                        }
                                        for (int j_ = 0; j_ <= 3; j_++) {
                                            if (j + j_ >= 0 && j + j_ <= 9) {
                                                parent[i][j + j_].setAlpha(0);
                                            }
                                        }
                                    }
                                } else {
                                    if (sf::IntRect(parent[i][j].cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 0 && j <= 6 && parent[i][j + 0].isAvailable() && parent[i][j + 1].isAvailable() &&
                                            parent[i][j + 2].isAvailable() && parent[i][j + 3].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    } else if (j >= 1 && sf::IntRect(parent[i][j - 1].cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 1 && j <= 7 && parent[i][j - 1].isAvailable() && parent[i][j + 0].isAvailable() &&
                                            parent[i][j + 1].isAvailable() && parent[i][j + 2].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    } else if (j >= 2 && sf::IntRect(parent[i][j - 2].cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 2 && j <= 8 && parent[i][j - 2].isAvailable() && parent[i][j - 1].isAvailable() &&
                                            parent[i][j + 0].isAvailable() && parent[i][j + 1].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    } else if (j >= 3 && sf::IntRect(parent[i][j - 3].cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 3 && j <= 9 && parent[i][j - 3].isAvailable() && parent[i][j - 2].isAvailable() &&
                                            parent[i][j - 1].isAvailable() && parent[i][j + 0].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    }
                                }
                            } else {
                                parent[i][j].setAlpha(0);
                            }
                        } else if (GameField::diffY > 32.f * scale.y && GameField::diffY <= 64.f * scale.y) {
                            if ((sf::IntRect(cell.getGlobalBounds()).contains(mouse)) ||
                                sf::IntRect(parent[i][j - 1].cell.getGlobalBounds()).contains(mouse) ||
                                sf::IntRect(parent[i][j - 2].cell.getGlobalBounds()).contains(mouse) ||
                                sf::IntRect(parent[i][j + 1].cell.getGlobalBounds()).contains(mouse)) {
                                if (event.type == sf::Event::MouseButtonReleased) {
                                    if (sf::IntRect(cell.getGlobalBounds()).contains(mouse)) {
                                        if (parent.addShip(i, j - 1, GameField::shipSize)) {

                                        }
                                        for (int j_ = -1; j_ <= 2; j_++) {
                                            if (j + j_ >= 0 && j + j_ <= 9) {
                                                parent[i][j + j_].setAlpha(0);
                                            }
                                        }
                                    }
                                } else {
                                    if (sf::IntRect(parent[i][j].cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 1 && j <= 7 && parent[i][j - 1].isAvailable() && parent[i][j + 0].isAvailable() &&
                                            parent[i][j + 1].isAvailable() && parent[i][j + 2].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    } else if (j >= 1 && sf::IntRect(parent[i][j - 1].cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 2 && j <= 8 && parent[i][j - 2].isAvailable() && parent[i][j - 1].isAvailable() &&
                                            parent[i][j + 0].isAvailable() && parent[i][j + 1].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    } else if (j >= 2 && sf::IntRect(parent[i][j - 2].cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 3 && j <= 9 && parent[i][j - 3].isAvailable() && parent[i][j - 2].isAvailable() &&
                                            parent[i][j - 1].isAvailable() && parent[i][j + 0].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    } else if (j <= 8 && sf::IntRect(parent[i][j + 1].cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 0 && j <= 6 && parent[i][j + 0].isAvailable() && parent[i][j + 1].isAvailable() &&
                                            parent[i][j + 2].isAvailable() && parent[i][j + 3].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    }
                                }
                            } else {
                                parent[i][j].setAlpha(0);
                            }
                        } else if (GameField::diffY > 64.f * scale.y && GameField::diffY <= 96.f * scale.y) {
                            if ((sf::IntRect(cell.getGlobalBounds()).contains(mouse)) ||
                                sf::IntRect(parent[i][j - 1].cell.getGlobalBounds()).contains(mouse) ||
                                sf::IntRect(parent[i][j + 2].cell.getGlobalBounds()).contains(mouse) ||
                                sf::IntRect(parent[i][j + 1].cell.getGlobalBounds()).contains(mouse)) {
                                if (event.type == sf::Event::MouseButtonReleased) {
                                    if (sf::IntRect(cell.getGlobalBounds()).contains(mouse)) {
                                        if (parent.addShip(i, j - 2, GameField::shipSize)) {

                                        }
                                        for (int j_ = -2; j_ <= 1; j_++) {
                                            if (j + j_ >= 0 && j + j_ <= 9) {
                                                parent[i][j + j_].setAlpha(0);
                                            }
                                        }
                                    }
                                } else {
                                    if (sf::IntRect(parent[i][j].cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 2 && j <= 8 && parent[i][j - 2].isAvailable() && parent[i][j - 1].isAvailable() &&
                                            parent[i][j + 0].isAvailable() && parent[i][j + 1].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    } else if (j >= 1 && sf::IntRect(parent[i][j - 1].cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 3 && j <= 9 && parent[i][j - 3].isAvailable() && parent[i][j - 2].isAvailable() &&
                                            parent[i][j - 1].isAvailable() && parent[i][j + 0].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    } else if (j <= 7 && sf::IntRect(parent[i][j + 2].cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 0 && j <= 6 && parent[i][j + 0].isAvailable() && parent[i][j + 1].isAvailable() &&
                                            parent[i][j + 2].isAvailable() && parent[i][j + 3].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    } else if (j <= 8 && sf::IntRect(parent[i][j + 1].cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 1 && j <= 7 && parent[i][j - 1].isAvailable() && parent[i][j + 0].isAvailable() &&
                                            parent[i][j + 1].isAvailable() && parent[i][j + 2].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    }
                                }
                            } else {
                                parent[i][j].setAlpha(0);
                            }
                        } else {
                            if ((sf::IntRect(cell.getGlobalBounds()).contains(mouse)) ||
                                sf::IntRect(parent[i][j + 1].cell.getGlobalBounds()).contains(mouse) ||
                                sf::IntRect(parent[i][j + 2].cell.getGlobalBounds()).contains(mouse) ||
                                sf::IntRect(parent[i][j + 3].cell.getGlobalBounds()).contains(mouse)) {
                                if (event.type == sf::Event::MouseButtonReleased) {
                                    if (sf::IntRect(cell.getGlobalBounds()).contains(mouse)) {
                                        if (parent.addShip(i, j - 3, GameField::shipSize)) {

                                        }
                                        for (int j_ = -3; j_ <= 0; j_++) {
                                            if (j + j_ >= 0 && j + j_ <= 9) {
                                                parent[i][j + j_].setAlpha(0);
                                            }
                                        }
                                    }
                                } else {
                                    if (sf::IntRect(parent[i][j].cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 3 && j <= 9 && parent[i][j - 3].isAvailable() && parent[i][j - 2].isAvailable() &&
                                            parent[i][j - 1].isAvailable() && parent[i][j + 0].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    } else if (j <= 6 && sf::IntRect(parent[i][j + 3].cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 0 && j <= 6 && parent[i][j + 0].isAvailable() && parent[i][j + 1].isAvailable() &&
                                            parent[i][j + 2].isAvailable() && parent[i][j + 3].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    } else if (j <= 7 && sf::IntRect(parent[i][j + 2].cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 1 && j <= 7 && parent[i][j - 1].isAvailable() && parent[i][j + 0].isAvailable() &&
                                            parent[i][j + 1].isAvailable() && parent[i][j + 2].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    } else if (j <= 8 && sf::IntRect(parent[i][j + 1].cell.getGlobalBounds()).contains(mouse)) {
                                        if (j >= 2 && j <= 8 && parent[i][j - 2].isAvailable() && parent[i][j - 1].isAvailable() &&
                                            parent[i][j + 0].isAvailable() && parent[i][j + 1].isAvailable()) {
                                            parent[i][j].setAlpha(100);
                                        } else {
                                            parent[i][j].cell.setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                    }
                                }
                            } else {
                                parent[i][j].setAlpha(0);
                            }
                        }
                    }
                    break;
            }
            break;
        case INACTIVE:
            break;
    }
}

void GameFieldCell::draw() const {
    window->draw(cell);
}

void GameFieldCell::setAlpha(unsigned char alpha) {
    cell.setFillColor(sf::Color(255, 255, 255, alpha));
}

bool GameFieldCell::isAvailable() const{
    return availability;
}

void GameFieldCell::rmAvailability(){
    availability = false;
}

void GameFieldCell::addAvailability(){
    availability = true;
}


GameField::GameField(sf::Vector2<float> position_, sf::Vector2<float> scale_, GameFieldState state_, std::shared_ptr<sf::RenderWindow> window_) :
        ScreenObject(window_), field("Field1.bmp", position_, scale_, window_), aliveCount(10), cells(10), state(state_),
        scale(scale_), position(position_){
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
    for (auto& s: ship1){
        if (s.getCoords() != ship.getCoords()){
            s.updateAvailability(cells);
        }
    }
    for (auto& s: ship2){
        if (s.getCoords() != ship.getCoords()){
            s.updateAvailability(cells);
        }
    }
    for (auto& s: ship3){
        if (s.getCoords() != ship.getCoords()){
            s.updateAvailability(cells);
        }
    }
    for (auto& s: ship4){
        if (s.getCoords() != ship.getCoords()){
            s.updateAvailability(cells);
        }
    }

}

void GameField::clearAvailability(){
    for (auto& i: cells){
        for (auto& c: i){
            c.addAvailability();
        }
    }
}

void GameField::eventCheck(sf::Event &event) {
    clearAvailability();

    for (auto& s: ship1){
        if (s.eventCheck(event, cells, [this](Ship<1>& ship) { updateAvailability(ship);}, *this) == REMOVED) {
            return;
        }
        s.updateAvailability(cells);
    }

    for (auto& s: ship2){
        if (s.eventCheck(event, cells, [this](Ship<2>& ship) { updateAvailability(ship);}, *this) == REMOVED) {
            return;
        }
        s.updateAvailability(cells);
    }

    for (auto& s: ship3){
        if (s.eventCheck(event, cells, [this](Ship<3>& ship) { updateAvailability(ship);}, *this) == REMOVED) {
            return;
        }
        s.updateAvailability(cells);
    }

    for (auto& s: ship4){
        if (s.eventCheck(event, cells, [this](Ship<4>& ship) { updateAvailability(ship);}, *this) == REMOVED) {
            return;
        }
        s.updateAvailability(cells);
    }


    for (char i = 0; i < cells.size(); i++){
        for (char j = 0; j < cells[i].size(); j++) {
            cells[i][j].eventCheck(event, state, *this, i, j, scale);
        }
    }
}

void GameField::draw() const {
    field.draw();
    drawShips(ship1);
    drawShips(ship2);
    drawShips(ship3);
    drawShips(ship4);
    for (auto &i: cells) {
        for (auto &c: i) {
            c.draw();
        }
    }
}

template<char N>
void GameField::drawShips(const std::list<Ship<N>> &ships) const {
    for (auto &ship: ships) {
        ship.draw();
    }
}

bool GameField::addShip(char i, char j, char shipType) {
    if (cells[i][j].isAvailable()) {
        auto newPosition = sf::Vector2<float>(position.x + (33.f * static_cast<float>(i) + 16.f) * scale.x,
                                              position.y + (33.f * static_cast<float>(j) + 16.f) * scale.y);
        switch (shipType) {
            case 1:
                if (cells[i][j].isAvailable()){
                    ship1.emplace_back(scale, std::pair{i, j}, newPosition, window);
                } else {
                    return false;
                }
                break;
            case 2:
                if (j <= 8 && cells[i][j].isAvailable() && cells[i][j+1].isAvailable()){
                    ship2.emplace_back(scale, std::pair{i, j}, newPosition, window);
                } else {
                    return false;
                }
                break;
            case 3:
                if (j <= 7 && cells[i][j].isAvailable() && cells[i][j+1].isAvailable() && cells[i][j+2].isAvailable()){
                    ship3.emplace_back(scale, std::pair{i, j}, newPosition, window);
                } else {
                    return false;
                }
                break;
            case 4:
                if (j <= 6 && cells[i][j].isAvailable() && cells[i][j+1].isAvailable() && cells[i][j+2].isAvailable() && cells[i][j+3].isAvailable()){
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

void GameField::removeShip(std::pair<char, char> coords, char shipType) {
    switch (shipType) {
        case 1:
            for (auto it = ship1.begin(); it != ship1.end(); it++){
                if (it->getCoords().first == coords.first && it->getCoords().second == coords.second){
                    clearAvailability();
                    ship1.erase(it);
                    return;
                }
            }
            break;
        case 2:
            for (auto it = ship2.begin(); it != ship2.end(); it++){
                if (it->getCoords().first == coords.first && it->getCoords().second == coords.second){
                    ship2.erase(it);
                    return;
                }
            }
            break;
        case 3:
            for (auto it = ship3.begin(); it != ship3.end(); it++){
                if (it->getCoords().first == coords.first && it->getCoords().second == coords.second){
                    ship3.erase(it);
                    return;
                }
            }
            break;
        case 4:
            for (auto it = ship4.begin(); it != ship4.end(); it++){
                if (it->getCoords().first == coords.first && it->getCoords().second == coords.second){
                    ship4.erase(it);
                    return;
                }
            }
            break;
    }
}

std::vector<GameFieldCell>& GameField::operator[](size_t i){
    if (i <= cells.size()) {
        return cells[i];
    }
    throw std::runtime_error("Invalid index in GameField!\n");
}


