#include "battle_ship_game.h"

const float BattleShipGame::WIDTH = 1920;
const float BattleShipGame::HEIGHT = 1080;

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


BattleShipGame::BattleShipGame(): server(std::make_unique<sf::TcpSocket>()), screen(sf::VideoMode::getDesktopMode()), status(LOGIN),
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
    titles["ship1Amount"] = std::make_unique<Title>(std::string("x" + std::to_string(4 - Ship<1>::aliveShips)),
                                                    sf::Vector2<float>{static_cast<float>(screen.width) * 0.5f + 50.0f,
                                                                       static_cast<float>(screen.height) * 0.11f},
                                                    window, 60);
    titles["ship2Amount"] = std::make_unique<Title>(std::string("x" + std::to_string(3 - Ship<2>::aliveShips)),
                                                    sf::Vector2<float>{static_cast<float>(screen.width) * 0.5f + 50.0f,
                                                                       static_cast<float>(screen.height) * 0.24f},
                                                    window, 60);
    titles["ship3Amount"] = std::make_unique<Title>(std::string("x" + std::to_string(2 - Ship<3>::aliveShips)),
                                                    sf::Vector2<float>{static_cast<float>(screen.width) * 0.5f + 50.0f,
                                                                       static_cast<float>(screen.height) * 0.42f},
                                                    window, 60);
    titles["ship4Amount"] = std::make_unique<Title>(std::string("x" + std::to_string(1 - Ship<4>::aliveShips)),
                                                    sf::Vector2<float>{static_cast<float>(screen.width) * 0.5f + 50.0f,
                                                                       static_cast<float>(screen.height) * 0.65f},
                                                    window, 60);
    buttons["startBattle"] = std::make_unique<Button>(float(screen.width) * 0.75f, float(screen.height) * 0.85f, screenScale,
                                                      [this] { startBattle(true); }, window, "start", 40, beige);
    fields["myField"] = std::make_unique<GameField>(sf::Vector2<float>{static_cast<float>(screen.width) * 0.05f,
                                                                                static_cast<float>(screen.height) * 0.2f},
                                                    sf::Vector2<float>{screenScale.x * 1.75f, screenScale.y * 1.75f},
                                                    PLACEMENT, window);
    fields["enemyField"] = std::make_unique<GameField>(sf::Vector2<float>{static_cast<float>(screen.width) * 0.65f,
                                                                                   static_cast<float>(screen.height) * 0.2f},
                                                    sf::Vector2<float>{screenScale.x * 1.75f, screenScale.y * 1.75f},
                                                    INACTIVE, window);
    dragDropShips["placement"] = std::make_unique<DraggableAndDroppableShips>(screenScale * 1.75f, screen, window);

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
                    fields["myField"]->eventCheck(event);
                    fields["enemyField"]->eventCheck(event);
                    buttons["mainMenu"]->eventCheck(event);
                    buttons["startBattle"]->eventCheck(event);
                    dragDropShips["placement"]->eventCheck(event);
                    break;
                case IN_SP_GAME:
                    fields["myField"]->eventCheck(event);
                    fields["enemyField"]->eventCheck(event);
                    buttons["mainMenu"]->eventCheck(event);
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
                buttons["startBattle"]->draw();

                titles["ship1Amount"]->setText("x" + std::to_string(4 - Ship<1>::aliveShips));
                titles["ship2Amount"]->setText("x" + std::to_string(3 - Ship<2>::aliveShips));
                titles["ship3Amount"]->setText("x" + std::to_string(2 - Ship<3>::aliveShips));
                titles["ship4Amount"]->setText("x" + std::to_string(1 - Ship<4>::aliveShips));

                titles["ship4Amount"]->draw();
                titles["ship3Amount"]->draw();
                titles["ship2Amount"]->draw();
                titles["ship1Amount"]->draw();

                fields["myField"]->draw();
                fields["enemyField"]->draw();
                dragDropShips["placement"]->draw();

                break;
            case IN_SP_GAME:
                buttons["mainMenu"]->draw();
                fields["myField"]->draw();
                fields["enemyField"]->draw();
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

void BattleShipGame::startBattle(bool singlePlayer){
    if (Ship<1>::aliveShips == 4 && Ship<2>::aliveShips == 3 && Ship<3>::aliveShips == 2 && Ship<4>::aliveShips == 1) {
        status = (singlePlayer ? IN_SP_GAME : IN_MP_GAME);
        fields["myField"]->setState(INACTIVE);
        fields["enemyField"]->setState(GAME);
    }
}
