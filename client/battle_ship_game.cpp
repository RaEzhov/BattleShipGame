#include "battle_ship_game.h"

#include <iostream>

const float BattleShipGame::WIDTH = 1920;
const float BattleShipGame::HEIGHT = 1080;

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

void BattleShipGame::loadTextures(){
    buttons["exit"] = std::make_unique<Button>(float(screen.width) * 0.05f, float(screen.height) * 0.85f, screenScale, nullptr, window,
                                               std::string(RESOURCES_PATH) + "exit.png");

    buttons["login"] = std::make_unique<Button>(float(screen.width) * 0.22f, float(screen.height) * 0.6f, screenScale * 0.5f,
                                                [this] { loginFunc(); }, window, std::string(RESOURCES_PATH) + "exit.png");

    entries["login"] = std::make_unique<Entry>(sf::Vector2{float(screen.width) * 0.2f, float(screen.height) * 0.4f}, 12,
                                               window, 24);

    entries["password"] = std::make_unique<Entry>(sf::Vector2{float(screen.width) * 0.2f, float(screen.height) * 0.5f}, 12,
                                                  window, 24);

    buttons["single"] = std::make_unique<Button>(static_cast<float>(screen.width)*0.05f, static_cast<float>(screen.height)*0.15f, screenScale,
                                                 [this]{singlePlayerFunc();}, window, std::string(RESOURCES_PATH)+ "exit.png");
    buttons["multi"] = std::make_unique<Button>(static_cast<float>(screen.width)*0.05f, static_cast<float>(screen.height)*0.35f, screenScale,
                                                [this]{multiPlayerFunc();}, window, std::string(RESOURCES_PATH)+ "exit.png");
    pictures["background"] = std::make_unique<Picture>("mainMenu.jpg",
                                                       sf::Vector2<float>{0,0}, screenScale, window);
    pictures["battleShipText"] = std::make_unique<Picture>("battleship.png",
                                                           sf::Vector2<float>(float(screen.width) * 0.35f, float(screen.height) * 0.85f),
                                                           screenScale, window);

    titles["login"] = std::make_unique<Title>(std::string("Login:"),
                                              sf::Vector2<float>{static_cast<float>(screen.width)*0.2f + 10.0f,
                                                             static_cast<float>(screen.height)*0.4f - 30.0f}, window);
    titles["password"] = std::make_unique<Title>(std::string("Password:"),
                                                 sf::Vector2<float>{static_cast<float>(screen.width)*0.2f + 10.0f,
                                                                static_cast<float>(screen.height)*0.5f - 30.0f}, window);
}

void BattleShipGame::mainLoop() {
    sf::Event event{};
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
        status = MAIN_MENU;
    }
}

void BattleShipGame::singlePlayerFunc() {
    status = IN_SP_MENU;
}

void BattleShipGame::multiPlayerFunc() {

}
