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


BattleShipGame::BattleShipGame(): server(std::make_unique<sf::TcpSocket>()), screen(sf::VideoMode::getDesktopMode()),
                                   window(std::make_shared<sf::RenderWindow>(sf::VideoMode::getDesktopMode(), "Battleship", sf::Style::Fullscreen)),
                                   screenScale(static_cast<float>(screen.width) / WIDTH, static_cast<float>(screen.height) / HEIGHT) {
    // Connection to server
    if (server->connect(Config::instance().ip, Config::instance().port) != sf::Socket::Done) {
        throw std::runtime_error("Connection error!\n");
    }

    // Set up framerate limit
    window->setFramerateLimit(Config::instance().framerate);

    // Cursor init
    cursorTex.loadFromFile(Config::instance().resources + "cursor.png");
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

    buttons["login"] = std::make_unique<Button>(float(screen.width) * 0.085f, float(screen.height) * 0.6f, screenScale * 0.7f,
                                                [this] { loginFunc(); }, window, "sign in", 40, beige);

    std::string login, password;

    std::ifstream cache;
    cache.open("cache");
    cache >> login >> password;
    cache.close();

    entries["login"] = std::make_unique<Entry>(sf::Vector2{float(screen.width) * 0.08f, float(screen.height) * 0.37f}, 12,
                                               window, 24, true, [this] { loginFunc(); }, login);

    entries["password"] = std::make_unique<Entry>(sf::Vector2{float(screen.width) * 0.08f, float(screen.height) * 0.47f},
                                                  12, window, 24, true, [this] { loginFunc(); }, password);

    buttons["register"] = std::make_unique<Button>(float(screen.width) * 0.085f, float(screen.height) * 0.7f, screenScale * 0.7f,
                                                   [this] { registerFunc(); }, window, "sign up", 40, beige);

    pictures["loginTable"] = std::make_unique<Picture>("loginTable.png", sf::Vector2<float>{screen.width * 0.04f, 0},
                                                       screenScale * 3.6f, window);

    buttons["single"] = std::make_unique<Button>(static_cast<float>(screen.width) * 0.05f, static_cast<float>(screen.height) * 0.25f,
                                                 screenScale, [this] { singlePlayerFunc(); }, window, "singleplayer", 25, beige);
    buttons["multi"] = std::make_unique<Button>(static_cast<float>(screen.width) * 0.05f, static_cast<float>(screen.height) * 0.45f,
                                                screenScale, [this] { multiPlayerFunc(); }, window, "multiplayer", 25, beige);
    pictures["background"] = std::make_unique<Picture>("mainMenu.jpg",
                                                       sf::Vector2<float>{0, 0}, screenScale, window);
    pictures["battleShipText"] = std::make_unique<Picture>("battleship.png",
                                                           sf::Vector2<float>(float(screen.width) * 0.35f,
                                                                              float(screen.height) * 0.85f),
                                                           screenScale, window);

    titles["login"] = std::make_unique<Title>(std::string("Login:"),
                                              sf::Vector2<float>{static_cast<float>(screen.width) * 0.08f + 10.0f,
                                                                 static_cast<float>(screen.height) * 0.37f - 30.0f},
                                              window);
    titles["password"] = std::make_unique<Title>(std::string("Password:"),
                                                 sf::Vector2<float>{static_cast<float>(screen.width) * 0.08f + 10.0f,
                                                                    static_cast<float>(screen.height) * 0.47f - 30.0f},
                                                 window);
    buttons["mainMenu"] = std::make_unique<Button>(float(screen.width) * 0.05f, float(screen.height) * 0.85f, screenScale,
                                                   [this] { mainMenu(); }, window, "back", 40, beige);
    titles["ship1Amount"] = std::make_unique<Title>(std::string("x" + std::to_string(4 - Ship<1>::aliveShips)),
                                                    sf::Vector2<float>{static_cast<float>(screen.width) * 0.5f + 50.0f,
                                                                       static_cast<float>(screen.height) * 0.17f},
                                                    window, 60);
    titles["ship2Amount"] = std::make_unique<Title>(std::string("x" + std::to_string(3 - Ship<2>::aliveShips)),
                                                    sf::Vector2<float>{static_cast<float>(screen.width) * 0.5f + 50.0f,
                                                                       static_cast<float>(screen.height) * 0.31f},
                                                    window, 60);
    titles["ship3Amount"] = std::make_unique<Title>(std::string("x" + std::to_string(2 - Ship<3>::aliveShips)),
                                                    sf::Vector2<float>{static_cast<float>(screen.width) * 0.5f + 50.0f,
                                                                       static_cast<float>(screen.height) * 0.48f},
                                                    window, 60);
    titles["ship4Amount"] = std::make_unique<Title>(std::string("x" + std::to_string(1 - Ship<4>::aliveShips)),
                                                    sf::Vector2<float>{static_cast<float>(screen.width) * 0.5f + 50.0f,
                                                                       static_cast<float>(screen.height) * 0.71f},
                                                    window, 60);
    buttons["startBattle"] = std::make_unique<Button>(float(screen.width) * 0.74f, float(screen.height) * 0.85f, screenScale,
                                                      [this] { startBattle(true); }, window, "start", 40, beige);
    buttons["randomPlace"] = std::make_unique<Button>(static_cast<float>(screen.width) * 0.425f, static_cast<float>(screen.height) * 0.88f,
                                                      screenScale * 0.7f, [this] { fields["myField"]->placeShipsRand(); }, window,
                                                      "randomly",
                                                      35, beige);
    fields["myField"] = std::make_unique<GameField>(sf::Vector2<float>{static_cast<float>(screen.width) * 0.04f,
                                                                       static_cast<float>(screen.height) * 0.25f},
                                                    sf::Vector2<float>{screenScale.x * 1.75f, screenScale.y * 1.75f},
                                                    PLACEMENT, window, [this] { changeSide(); });
    fields["enemyField"] = std::make_unique<GameField>(sf::Vector2<float>{static_cast<float>(screen.width) * 0.66f,
                                                                          static_cast<float>(screen.height) * 0.25f},
                                                       sf::Vector2<float>{screenScale.x * 1.75f, screenScale.y * 1.75f},
                                                       INACTIVE, window, [this] { changeSide(); });
    dragDropShips["placement"] = std::make_unique<DraggableAndDroppableShips>(screenScale * 1.75f, screen, window);
    pictures["gameBackground"] = std::make_unique<Picture>("gameMenu.jpg", sf::Vector2<float>{0, 0}, screenScale, window);
    titles["myName"] = std::make_unique<Title>(user.login, sf::Vector2<float>{static_cast<float>(screen.width) * 0.02f,
                                                                              static_cast<float>(screen.height) * 0.03f},
                                               window, 80, sf::Color::White);
    titles["myLevel"] = std::make_unique<Title>(user.getRatingStr(), sf::Vector2<float>{static_cast<float>(screen.width) * 0.02f,
                                                                                        static_cast<float>(screen.height) * 0.1f},
                                                window, 40, sf::Color::White);
    titles["enemyName"] = std::make_unique<Title>("computer", sf::Vector2<float>{static_cast<float>(screen.width) * 0.75f,
                                                                                 static_cast<float>(screen.height) * 0.03f},
                                                  window, 80, sf::Color::White);
    titles["enemyLevel"] = std::make_unique<Title>("level: -", sf::Vector2<float>{static_cast<float>(screen.width) * 0.9f,
                                                                                  static_cast<float>(screen.height) * 0.1f},
                                                   window, 40, sf::Color::White);
    titles["end"] = std::make_unique<Title>("", sf::Vector2<float>(static_cast<float>(screen.width)*0.5f,
                                                                             static_cast<float>(screen.height)*0.5f), window, 100,
                                               sf::Color::White);

}

void BattleShipGame::mainLoop() {
    sf::Event event{};

    while (window->isOpen()) {
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window->close();
            }
            switch (user.status) {
                case LOGIN:
                    entries["login"]->eventCheck(event);
                    entries["password"]->eventCheck(event);
                    buttons["login"]->eventCheck(event);
                    buttons["exit"]->eventCheck(event);
                    buttons["register"]->eventCheck(event);
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
                    buttons["randomPlace"]->eventCheck(event);
                    break;
                case IN_SP_GAME:
                    if (user.myMove) {
                        user.wait = false;
                        fields["enemyField"]->eventCheck(event);
                    } else if (!user.wait) {
                        std::thread wait([this] { fields["myField"]->selfMove(); });
                        wait.detach();
                        user.wait = true;
                    }
                    if (fields["myField"]->getAliveShips() == 0){
                        finishBattle(false);
                    }
                    if (fields["enemyField"]->getAliveShips() == 0){
                        finishBattle(true);
                    }

                    fields["myField"]->eventCheck(event);
                    buttons["mainMenu"]->eventCheck(event);
                    break;
                case END_OF_GAME:
                    buttons["mainMenu"]->eventCheck(event);
                    break;
                default:
                    std::cerr << "Wrong status\n";
            }
        }

        switch (user.status) {
            case LOGIN:
                pictures["background"]->draw();
                pictures["loginTable"]->draw();
                pictures["battleShipText"]->draw();
                entries["login"]->draw();
                titles["login"]->draw();
                entries["password"]->draw(true);
                titles["password"]->draw();
                buttons["login"]->draw();
                buttons["register"]->draw();
                buttons["exit"]->draw();
                break;
            case MAIN_MENU:
                pictures["background"]->draw();
                titles["myName"]->draw();
                titles["myLevel"]->draw();
                pictures["battleShipText"]->draw();
                buttons["single"]->draw();
                buttons["multi"]->draw();
                buttons["exit"]->draw();
                break;
            case IN_SP_MENU:
                pictures["gameBackground"]->draw();
                buttons["mainMenu"]->draw();
                buttons["startBattle"]->draw();
                buttons["randomPlace"]->draw();

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

                titles["myName"]->draw();
                titles["myLevel"]->draw();
                titles["enemyName"]->draw();
                titles["enemyLevel"]->draw();

                break;
            case IN_SP_GAME:
                pictures["gameBackground"]->draw();
                buttons["mainMenu"]->draw();

                fields["myField"]->draw();
                fields["enemyField"]->draw();

                titles["myName"]->draw();
                titles["myLevel"]->draw();
                titles["enemyName"]->draw();
                titles["enemyLevel"]->draw();
                break;
            case END_OF_GAME:

                pictures["gameBackground"]->draw();
                buttons["mainMenu"]->draw();

                fields["myField"]->draw();
                fields["enemyField"]->draw();

                titles["myName"]->draw();
                titles["myLevel"]->draw();
                titles["enemyName"]->draw();
                titles["enemyLevel"]->draw();
                titles["end"]->draw();

                break;
            default:
                std::cerr << "Wrong status\n";
        }
        cursor.setPosition(static_cast<float>(sf::Mouse::getPosition().x), static_cast<float>(sf::Mouse::getPosition().y));
        window->draw(cursor);
        window->display();
    }
}

std::string crypt(const std::string string, const std::string key){
    std::string result;
    for (int i = 0; i < string.size(); i++){
        result.append(std::to_string(static_cast<int>(string[i]) + static_cast<int>(key[i % key.size()])));
    }
    return result;
}

void BattleShipGame::loginFunc() {
    auto login = entries["login"]->getStr();
    auto password = crypt(entries["password"]->getStr(), login);
    sf::Packet packet;
    packet << login << password;
    server->send(packet);
    packet.clear();
    bool authDone;
    server->receive(packet);
    packet >> authDone;
    if (authDone) {
        packet.clear();
        server->receive(packet);
        std::pair<unsigned int, unsigned int> idRating;
        packet >> idRating.first >> idRating.second;
        user.init(login, idRating.first, idRating.second);

        mainMenu();
    }
}

void BattleShipGame::registerFunc() {
    auto login = entries["login"]->getStr();
    auto password = crypt(entries["password"]->getStr(), login);
    login = '@' + login;
    sf::Packet packet;
    packet << login << password;
    server->send(packet);
    packet.clear();
    bool authDone;
    server->receive(packet);
    packet >> authDone;
    if (authDone) {
        packet.clear();
        server->receive(packet);
        std::pair<unsigned int, unsigned int> idRating;
        packet >> idRating.first >> idRating.second;
        login.erase(login.begin());
        user.init(login, idRating.first, idRating.second);
        mainMenu();
    }
}

void BattleShipGame::mainMenu() {
    // Saving cache login and password
    std::ofstream cache;
    cache.open("cache");
    cache << entries["login"]->getStr() << '\n' << entries["password"]->getStr();
    cache.close();

    titles["myName"]->setText(user.login);
    titles["myName"]->setColor(sf::Color::Black);
    titles["myLevel"]->setText(user.getRatingStr());
    titles["myLevel"]->setColor(sf::Color::Black);
    fields["myField"]->clearAvailability();
    fields["myField"]->clearColors();
    fields["enemyField"]->clearAvailability();
    fields["enemyField"]->clearColors();
    user.status = MAIN_MENU;
}

void BattleShipGame::singlePlayerFunc() {
    user.status = IN_SP_MENU;
    titles["myName"]->setColor(sf::Color::White);
    titles["myLevel"]->setColor(sf::Color::White);
    titles["enemyName"]->setPosition(sf::Vector2<float>{static_cast<float>(screen.width) * 0.98f - titles["enemyName"]->getSize().width,
                                                        static_cast<float>(screen.height) * 0.03f});
    titles["enemyLevel"]->setPosition(sf::Vector2<float>{static_cast<float>(screen.width) * 0.98f - titles["enemyLevel"]->getSize().width,
                                                         static_cast<float>(screen.height) * 0.1f});
    fields["myField"]->setState(PLACEMENT);
    fields["enemyField"]->setState(INACTIVE);
    fields["myField"]->clearShips();
    fields["enemyField"]->clearShips();
}

void BattleShipGame::multiPlayerFunc() {

}

void BattleShipGame::startBattle(bool singlePlayer) {
    if (Ship<1>::aliveShips == 4 && Ship<2>::aliveShips == 3 && Ship<3>::aliveShips == 2 && Ship<4>::aliveShips == 1) {
        user.status = (singlePlayer ? IN_SP_GAME : IN_MP_GAME);
        if (singlePlayer) {
            fields["enemyField"]->placeShipsRand();
            fields["enemyField"]->clearAvailability(true);
            fields["myField"]->clearAvailability(true);
            fields["myField"]->setState(INACTIVE);
            fields["enemyField"]->setState(GAME);
        } else {

        }
    }
}

void BattleShipGame::finishBattle(bool meWin){
    fields["enemyField"]->setState(INACTIVE);
    user.status = END_OF_GAME;
    auto& ttl = titles["end"];
    if (meWin){
        ttl->setText("you win");
    } else {
        ttl->setText("you lose");
    }
    ttl->setPosition(sf::Vector2<float>(static_cast<float>(screen.width)/2.f - ttl->getSize().width/2.f,
                                                  static_cast<float>(screen.height)/2.f - ttl->getSize().height/2.f));
    //TODO send info to server
}

void BattleShipGame::changeSide() {
    user.myMove = !user.myMove;
}
