#include "battle_ship_game.h"

#include <iostream>

const float BattleShipGame::WIDTH = 1920;
const float BattleShipGame::HEIGHT = 1080;

char GameField::shipSize = 0;

template <char N>
std::array<sf::Texture, 5> Ship<N>::alive;
template <char N>
std::array<sf::Texture, 5> Ship<N>::injured;
template <char N>
std::array<sf::Texture, 5> Ship<N>::destroyed;


BattleShipGame::BattleShipGame() : server(std::make_unique<sf::TcpSocket>()), screen(sf::VideoMode::getDesktopMode()), status(LOGIN),
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

    GameField myField({100, 100}, {screenScale.x * 1.75f, screenScale.y * 1.75f}, GAME, window);

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


DraggableAndDroppableShips::DraggableAndDroppableShips(const sf::Vector2<float> &scale, const sf::VideoMode &screen,
                                                       std::shared_ptr<sf::RenderWindow> window_): ScreenObject(window_) {
    for (int i = 1; i <= 4; i++) {
        tShip[i].loadFromFile(std::string(RESOURCES_PATH) + "Ship" + std::to_string(i) + "_a.png");
        sShip[i].setTexture(tShip[i]);
        sShip[i].setScale(scale);
        startPos[i] = sf::Vector2<float>(
                static_cast<float>(screen.width) * 0.5f - sShip[i].getGlobalBounds().width / 2.0f,
                static_cast<float>(screen.height) * 0.25f + (static_cast<float>(i) - 1.0f) * sShip[i].getGlobalBounds().height / 2.0f);
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
        if (sf::IntRect(sShip[i].getGlobalBounds()).contains(sf::Mouse::getPosition(*window))) {
            if (event.type == sf::Event::MouseButtonPressed) {
                dragged[i] = true;
                GameField::shipSize = i;
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
        ScreenObject(window_), underShip(false) {
    cell.setSize({32, 32});
    cell.setScale(scale);
    cell.setFillColor(sf::Color(255, 255, 255, 0));
    cell.setPosition(position);
}

void GameFieldCell::setPosition(sf::Vector2<float> newPosition) {
    cell.setPosition(newPosition);
}

void GameFieldCell::eventCheck(sf::Event& event) {
    if (sf::IntRect(cell.getGlobalBounds()).contains(sf::Mouse::getPosition(*window))) {
        cell.setFillColor(sf::Color(255, 255, 255, 100));
    } else {
        cell.setFillColor(sf::Color(255, 255, 255, 0));
    }
}

void GameFieldCell::draw() const {
    window->draw(cell);
}


GameField::GameField(sf::Vector2<float> position, sf::Vector2<float> scale, GameFieldState state_, std::shared_ptr<sf::RenderWindow> window_) :
        ScreenObject(window_), field("Field1.bmp", position, scale, window_), aliveCount(10), cells(10), state(state_) {
    int row = 0, col = 0;
    const int rectSize = 32;
    for (auto &i: cells) {
        col = 0;
        for (int j = 0; j < 10; j++) {
            i.emplace_back(scale, position, window_);
            i[j].setPosition({position.x + static_cast<float>(rectSize * row + row) * scale.x,
                              position.y + static_cast<float>(rectSize * col + col) * scale.y});
            col++;
        }
        row++;
    }
}

void GameField::eventCheck(sf::Event &event) {
    switch (state) {
        case INACTIVE:
            break;
        case GAME:
            for (auto &i: cells) {
                for (auto &c: i) {
                    c.eventCheck(event);
                }
            }
            break;
        case PLACEMENT:
            break;
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
void GameField::drawShips(const std::vector<Ship<N>> &ships) const {
    for (auto &ship: ships) {
        ship.draw();
    }
}
