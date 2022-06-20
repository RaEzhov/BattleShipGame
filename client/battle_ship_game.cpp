// Copyright 2022 Roman Ezhov. Github: RaEzhov

#include <memory>
#include <thread>

#include "client/battle_ship_game.h"

std::pair<unsigned char, unsigned char> BattleShipGame::moveCoords = {0, 0};
bool BattleShipGame::sendMove = false;

const sf::Vector2<unsigned int> FHD(1920, 1080);

BattleShipGame::BattleShipGame() :
server(std::make_unique<sf::TcpSocket>()),
screen(sf::VideoMode::getDesktopMode()),
screenScale(screen.width / FHD.x, screen.height /FHD.y),
window(std::make_shared<sf::RenderWindow>(screen, "Battleship")),
WIDTH(window->getSize().x),
HEIGHT(window->getSize().y) {
  // Connection to server
  if (server->connect(Config::instance().ip,
                      Config::instance().port) != sf::Socket::Done) {
    throw std::runtime_error("Connection error!\n");
  }
  serverConnected = true;

  // Set up framerate limit
  window->setFramerateLimit(Config::instance().framerate);
  // Load all textures
  loadTextures();
  enemy.reset();

  if (!music.openFromFile(Config::instance().resources + "soundTrack.ogg")) {
    throw std::runtime_error("Can't open soundtrack\n");
  }
  music.setVolume(30);
  music.setLoop(true);

  music.play();
}

void BattleShipGame::loadTextures() {
  Ship<1>::loadTextures();
  Ship<2>::loadTextures();
  Ship<3>::loadTextures();
  Ship<4>::loadTextures();
  buttons["exit"] = std::make_unique<Button>(
      sf::Vector2{
        static_cast<float>(screen.width) * 0.05f,
        static_cast<float>(screen.height) * 0.85f},
     screenScale, nullptr, window, "exit", 40, beige);

  buttons["login"] = std::make_unique<Button>(
      sf::Vector2{
        static_cast<float>(screen.width) * 0.085f,
        static_cast<float>(screen.height) * 0.6f},
      screenScale * 0.7f, [this] { loginFunc(); }, window, "sign in", 40,
      beige);

  std::string login, password;

  std::ifstream cache;
  cache.open("cache");
  cache >> login >> password;
  cache.close();

  entries["login"] = std::make_unique<Entry>(
      sf::Vector2{
        static_cast<float>(screen.width) * 0.08f,
        static_cast<float>(screen.height) * 0.37f},
      screenScale, 12, window, 24, true, [this] { loginFunc(); }, login);

  entries["password"] = std::make_unique<Entry>(
      sf::Vector2{
        static_cast<float>(screen.width) * 0.08f,
        static_cast<float>(screen.height) * 0.47f},
      screenScale, 12, window, 24, true, [this] { loginFunc(); }, password);

  buttons["register"] = std::make_unique<Button>(
      sf::Vector2{
        static_cast<float>(screen.width) * 0.085f,
        static_cast<float>(screen.height) * 0.7f},
      screenScale * 0.7f, [this] { registerFunc(); }, window, "sign up", 40,
      beige);

  buttons["single"] = std::make_unique<Button>(
      sf::Vector2{
        static_cast<float>(screen.width) * 0.05f,
        static_cast<float>(screen.height) * 0.25f},
      screenScale, [this] { singlePlayerFunc(); }, window, "singleplayer", 25,
      beige);

  buttons["multi"] = std::make_unique<Button>(
      sf::Vector2{
        static_cast<float>(screen.width) * 0.05f,
        static_cast<float>(screen.height) * 0.45f},
      screenScale, [this] { multiPlayerLobby(); }, window, "multiplayer", 25,
      beige);

  pictures["background"] = std::make_unique<Picture>(
      "mainMenu.jpg", sf::Vector2<float>{0, 0}, screenScale, window);

  pictures["battleShipText"] = std::make_unique<Picture>(
      "battleship.png", sf::Vector2<float>(
          static_cast<float>(screen.width) * 0.35f,
          static_cast<float>(screen.height) * 0.85f),
      screenScale, window);

  titles["login"] = std::make_unique<Title>(
      std::string("Login:"), sf::Vector2<float>{
        static_cast<float>(screen.width) * 0.08f + 10.0f,
        static_cast<float>(screen.height) * 0.37f - 40.0f},
      screenScale, window, 30);

  titles["password"] = std::make_unique<Title>(
      std::string("Password:"),
      sf::Vector2<float>{static_cast<float>(screen.width) * 0.08f + 10.0f,
                         static_cast<float>(screen.height) * 0.47f - 40.0f},
      screenScale, window, 30);

  buttons["mainMenu"] = std::make_unique<Button>(
      sf::Vector2{static_cast<float>(screen.width) * 0.05f,
                  static_cast<float>(screen.height) * 0.85f},
      screenScale, [this] { mainMenu(); }, window, "back", 40, beige);

  titles["ship1Amount"] = std::make_unique<Title>(
      std::string("x" + std::to_string(4 - Ship<1>::aliveShips)),
      sf::Vector2<float>{
        static_cast<float>(screen.width) * 0.5f + 50.0f,
        static_cast<float>(screen.height) * 0.17f},
      screenScale, window, 60);

  titles["ship2Amount"] = std::make_unique<Title>(
      std::string("x" + std::to_string(3 - Ship<2>::aliveShips)),
      sf::Vector2<float>{static_cast<float>(screen.width) * 0.5f + 50.0f,
                         static_cast<float>(screen.height) * 0.31f},
      screenScale, window, 60);

  titles["ship3Amount"] = std::make_unique<Title>(
      std::string("x" + std::to_string(2 - Ship<3>::aliveShips)),
      sf::Vector2<float>{static_cast<float>(screen.width) * 0.5f + 50.0f,
                         static_cast<float>(screen.height) * 0.48f},
      screenScale, window, 60);

  titles["ship4Amount"] = std::make_unique<Title>(
      std::string("x" + std::to_string(1 - Ship<4>::aliveShips)),
      sf::Vector2<float>{static_cast<float>(screen.width) * 0.5f + 50.0f,
                         static_cast<float>(screen.height) * 0.71f},
      screenScale, window, 60);

  buttons["startBattle"] = std::make_unique<Button>(
      sf::Vector2{static_cast<float>(screen.width) * 0.74f,
                  static_cast<float>(screen.height) * 0.85f},
      screenScale, [this] { startBattle(); }, window, "start", 40, beige);

  buttons["randomPlace"] = std::make_unique<Button>(
      sf::Vector2{static_cast<float>(screen.width) * 0.425f,
                  static_cast<float>(screen.height) * 0.88f},
      screenScale * 0.7f, [this] { fields["myField"]->placeShipsRand(); },
      window, "randomly", 35, beige);

  fields["myField"] = std::make_unique<GameField>(
      sf::Vector2<float>{static_cast<float>(screen.width) * 0.04f,
                         static_cast<float>(screen.height) * 0.25f},
      sf::Vector2<float>{screenScale.x * 1.75f, screenScale.y * 1.75f},
      PLACEMENT, window, [this] { changeSide(); });

  fields["enemyField"] = std::make_unique<GameField>(
      sf::Vector2<float>{static_cast<float>(screen.width) * 0.66f,
                         static_cast<float>(screen.height) * 0.25f},
      sf::Vector2<float>{screenScale.x * 1.75f, screenScale.y * 1.75f},
      INACTIVE, window, [this] { changeSide(); });

  dragDropShips["placement"] = std::make_unique<DragNDropShp>(
      screenScale * 1.75f, screen, window);
  pictures["gameBackground"] = std::make_unique<Picture>(
      "gameMenu.jpg", sf::Vector2<float>{0, 0}, screenScale, window);

  titles["myName"] = std::make_unique<Title>(
      user.login, sf::Vector2<float>{
        static_cast<float>(screen.width) * 0.02f,
        static_cast<float>(screen.height) * 0.025f},
      screenScale, window, 80, beige);

  titles["myLevel"] = std::make_unique<Title>(
      user.getRatingStr(), sf::Vector2<float>{
        static_cast<float>(screen.width) * 0.02f,
        static_cast<float>(screen.height) * 0.1f},
      screenScale, window, 40, beige);

  titles["enemyName"] = std::make_unique<Title>(
      "computer", sf::Vector2<float>{
        static_cast<float>(screen.width) * 0.75f,
        static_cast<float>(screen.height) * 0.025f},
      screenScale, window, 80,
      beige);
  titles["enemyLevel"] = std::make_unique<Title>(
      "level: -", sf::Vector2<float>{
        static_cast<float>(screen.width) * 0.9f,
        static_cast<float>(screen.height) * 0.1f},
      screenScale, window, 40, beige);

    titles["end"] = std::make_unique<Title>(
        "", sf::Vector2<float>(
            static_cast<float>(screen.width) * 0.5f,
            static_cast<float>(screen.height) * 0.5f),
        screenScale, window, 100, sf::Color::White);

  pictures["end"] = std::make_unique<Picture>(
      "ribbon.png", sf::Vector2<float>{0, 0},
      screenScale * 10.f, window);

  pictures["lobby"] = std::make_unique<Picture>(
      "lobbyMenu.png", sf::Vector2<float>{0, 0},
      screenScale, window);

  pages["friends"] = std::make_unique<Pages>(
      sf::Vector2<float>{
        static_cast<float>(screen.width) * 0.3f,
        static_cast<float>(screen.height) * 0.1f},
      screenScale, window);

  entries["friends"] =
      std::make_unique<Entry>(
          sf::Vector2{
            static_cast<float>(screen.width) * 0.33f,
            static_cast<float>(screen.height) * 0.8f},
          screenScale, 12, window, 24, true, nullptr);

  buttons["addFriend"] = std::make_unique<Button>(
      sf::Vector2{
        static_cast<float>(screen.width) * 0.5f,
        static_cast<float>(screen.height) * 0.75f},
      screenScale * 0.6f, [this] { addFriend(); }, window, "add friend",
      20, beige);

  buttons["removeFriend"] = std::make_unique<Button>(
      sf::Vector2{
        static_cast<float>(screen.width) * 0.5f,
        static_cast<float>(screen.height) * 0.85f},
      screenScale * 0.6f, [this] { removeFriend(); }, window, "remove friend",
      20, beige);

  buttons["randomRival"] = std::make_unique<Button>(
      sf::Vector2{static_cast<float>(screen.width) * 0.74f,
                  static_cast<float>(screen.height) * 0.85f},
      screenScale, [this] { randomRival(); }, window, "random\nrival",
      30, beige);

  notifications = std::make_unique<NotificationPool>(screenScale, window);

  buttons["ready"] = std::make_unique<Button>(
      sf::Vector2{static_cast<float>(screen.width) * 0.74f,
                  static_cast<float>(screen.height) * 0.85f},
      screenScale, [this] { startBattle(); }, window, "ready",
      40, beige);

  titles["friends"] = std::make_unique<Title>(
      "friends", sf::Vector2<float>{static_cast<float>(screen.width) * 0.4f,
                                   static_cast<float>(screen.height) * 0.05f},
      screenScale, window, 80, beige);
}

void BattleShipGame::mainLoop() {
  sf::Event event{};

  while (window->isOpen()) {
    while (window->pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window->close();
      }
      if (event.type == sf::Event::Resized) {
        sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
        window->setView(sf::View(visibleArea));
        screen = sf::VideoMode(event.size.width, event.size.height);
        screenScale = sf::Vector2<float>{
          screen.width / static_cast<float>(FHD.x),
          screen.height / static_cast<float>(FHD.y)};
        loadTextures();
      }

      notifications->eventCheck(event);
      if (serverConnected) {
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
            if (fields["myField"]->getAliveShips() == 0) {
              finishBattle(false);
            }
            if (fields["enemyField"]->getAliveShips() == 0) {
              finishBattle(true);
            }

            fields["myField"]->eventCheck(event);
            buttons["mainMenu"]->eventCheck(event);
            break;
          case END_OF_GAME:
            buttons["mainMenu"]->eventCheck(event);
            break;
          case IN_MP_LOBBY:
            buttons["mainMenu"]->eventCheck(event);
            pages["friends"]->eventCheck(event);
            entries["friends"]->eventCheck(event);
            buttons["addFriend"]->eventCheck(event);
            buttons["removeFriend"]->eventCheck(event);
            buttons["randomRival"]->eventCheck(event);
            break;
          case IN_MP_MENU:fields["myField"]->eventCheck(event);
            fields["enemyField"]->eventCheck(event);
            buttons["mainMenu"]->eventCheck(event);
            buttons["ready"]->eventCheck(event);
            dragDropShips["placement"]->eventCheck(event);
            buttons["randomPlace"]->eventCheck(event);
            break;
          case IN_MP_GAME:
            if (user.myMove) {
              user.wait = false;
              fields["enemyField"]->eventCheck(event);
            } else if (!user.wait) {
              user.wait = true;
            }
            fields["myField"]->eventCheck(event);
            buttons["mainMenu"]->eventCheck(event);

            if (BattleShipGame::sendMove) {
              BattleShipGame::sendMove = false;
              sf::Packet packet;
              packet << DO_MOVE << enemy.id << BattleShipGame::moveCoords.first
                     << BattleShipGame::moveCoords.second;
              server->send(packet);
            }

            // if ships on field (battle already starts)
            if (fields["enemyField"]->ship1.size() == 4
                && fields["myField"]->ship1.size() == 4) {
              if (fields["myField"]->getAliveShips() == 0) {
                finishBattle(false);
              }
              if (fields["enemyField"]->getAliveShips() == 0) {
                finishBattle(true);
              }
            }
            break;
          default:std::cerr << "Wrong status\n";
        }
      }
    }
    window->clear();
    switch (user.status) {
      case LOGIN:
        pictures["background"]->draw();
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

        titles["ship1Amount"]->setText(
            "x" + std::to_string(4 - Ship<1>::aliveShips));
        titles["ship2Amount"]->setText(
            "x" + std::to_string(3 - Ship<2>::aliveShips));
        titles["ship3Amount"]->setText(
            "x" + std::to_string(2 - Ship<3>::aliveShips));
        titles["ship4Amount"]->setText(
            "x" + std::to_string(1 - Ship<4>::aliveShips));

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
        pictures["end"]->draw();
        titles["end"]->draw();

        break;
      case IN_MP_LOBBY:
        pictures["lobby"]->draw();
        buttons["mainMenu"]->draw();
        pages["friends"]->draw();
        entries["friends"]->draw();
        buttons["addFriend"]->draw();
        buttons["removeFriend"]->draw();
        buttons["randomRival"]->draw();
        titles["friends"]->draw();
        break;
      case IN_MP_MENU:
        pictures["gameBackground"]->draw();
        buttons["mainMenu"]->draw();
        buttons["ready"]->draw();
        buttons["randomPlace"]->draw();

        titles["ship1Amount"]->setText(
            "x" + std::to_string(4 - fields["myField"]->ship1.size()));
        titles["ship2Amount"]->setText(
            "x" + std::to_string(3 - fields["myField"]->ship2.size()));
        titles["ship3Amount"]->setText(
            "x" + std::to_string(2 - fields["myField"]->ship3.size()));
        titles["ship4Amount"]->setText(
            "x" + std::to_string(1 - fields["myField"]->ship4.size()));

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
      case IN_MP_GAME:
        pictures["gameBackground"]->draw();
        buttons["mainMenu"]->draw();

        fields["myField"]->draw();
        fields["enemyField"]->draw();

        titles["myName"]->draw();
        titles["myLevel"]->draw();
        titles["enemyName"]->draw();
        titles["enemyLevel"]->draw();
        break;
      default:std::cerr << "Wrong status\n";
    }
    notifications->draw();
    window->display();
  }
}

std::string crypt(const std::string& string, const std::string& key) {
  std::string result;
  for (int i = 0; i < string.size(); i++) {
    result.append(std::to_string(static_cast<int>(string[i])
    + static_cast<int>(key[i % key.size()])));
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
  checkServer(server->receive(packet));

  packet >> authDone;
  if (authDone) {
    packet.clear();
    checkServer(server->receive(packet));
    std::pair<unsigned int, unsigned int> idRating;
    packet >> idRating.first >> idRating.second;
    user.init(login, idRating.first, idRating.second);
    std::thread listener([this] { serverListener(); });
    listener.detach();
    mainMenu();
  } else {
    notifications->addNotification("Incorrect\nlogin or password");
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
  checkServer(server->receive(packet));

  packet >> authDone;
  if (authDone) {
    packet.clear();
    checkServer(server->receive(packet));
    std::pair<unsigned int, unsigned int> idRating;
    packet >> idRating.first >> idRating.second;
    login.erase(login.begin());
    user.init(login, idRating.first, idRating.second);
    std::thread listener([this] { serverListener(); });
    listener.detach();
    mainMenu();
  } else {
    notifications->addNotification("such user\nalready exists");
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
  enemy.reset();
  fields["enemyField"]->clearAvailability();
  fields["enemyField"]->clearColors();
  user.status = MAIN_MENU;
}

void BattleShipGame::setEnemyTitles() {
  titles["enemyName"]->setText(enemy.getNameStr());
  titles["enemyLevel"]->setText(enemy.getRatingStr());

  titles["enemyName"]->setPosition(sf::Vector2<float>{
      static_cast<float>(screen.width) * 0.98f
          - titles["enemyName"]->getSize().width,
      static_cast<float>(screen.height) * 0.025f});

  titles["enemyLevel"]->setPosition(sf::Vector2<float>{
      static_cast<float>(screen.width) * 0.98f
          - titles["enemyLevel"]->getSize().width,
      static_cast<float>(screen.height) * 0.1f});
}

void BattleShipGame::singlePlayerFunc() {
  user.status = IN_SP_MENU;
  titles["myName"]->setColor(beige);
  titles["myLevel"]->setColor(beige);
  setEnemyTitles();

  pictures["end"]->setPosition(sf::Vector2<float>{
      static_cast<float>(screen.width) * 0.5f
          - pictures["end"]->getSize().width * 0.5f,
      static_cast<float>(screen.height) * 0.5f
          - pictures["end"]->getSize().height * 0.5f +
          static_cast<float>(screen.height) * 0.1f});

  fields["myField"]->setState(PLACEMENT);
  fields["enemyField"]->setState(INACTIVE);
  fields["myField"]->clearShips();
  fields["enemyField"]->clearShips();
}

void BattleShipGame::multiPlayerLobby() {
  sf::Packet packet;
  packet << GET_FRIENDS;
  server->send(packet);
  user.status = IN_MP_LOBBY;
}

void BattleShipGame::multiPlayerFunc(const std::string &enemy_) {
  sf::Packet packet;
  packet << WANT_FRIEND_PLAY << enemy_;
  server->send(packet);

  titles["myName"]->setColor(beige);
  titles["myLevel"]->setColor(beige);
  setEnemyTitles();
  pictures["end"]->setPosition(sf::Vector2<float>{
      static_cast<float>(screen.width) * 0.5f
          - pictures["end"]->getSize().width * 0.5f,
      static_cast<float>(screen.height) * 0.5f
          - pictures["end"]->getSize().height * 0.5f +
          static_cast<float>(screen.height) * 0.1f});
  fields["myField"]->setState(PLACEMENT);
  fields["enemyField"]->setState(ENEMY_INACTIVE);
  fields["myField"]->clearShips();
  fields["enemyField"]->clearShips();

  notifications->addNotification("waiting for enemy");
  user.status = IN_MP_MENU;
}

void BattleShipGame::startBattle() {
  if (fields["myField"]->ship1.size() == 4
      && fields["myField"]->ship2.size() == 3
      && fields["myField"]->ship3.size() == 2
      && fields["myField"]->ship4.size() == 1) {
    if (user.status == IN_SP_MENU) {
      fields["enemyField"]->placeShipsRand();
      fields["enemyField"]->clearAvailability(true);
      fields["myField"]->clearAvailability(true);
      fields["myField"]->setState(INACTIVE);
      fields["enemyField"]->setState(GAME);
      user.status = IN_SP_GAME;
    } else if (user.status == IN_MP_MENU) {
      // Enemy not found
      if (enemy.getNameStr() == "-") {
        notifications->addNotification("your enemy did\nnot join");
        return;
      }

      sf::Packet packet;
      packet << ENEMY_FIELD << enemy.id;
      for (auto i : fields["myField"]->serializedField()) {
        packet << i;
      }
      checkServer(server->send(packet));
      fields["myField"]->setState(INACTIVE);
      fields["enemyField"]->setState(GAME);
      user.status = IN_MP_GAME;
    }
  } else {
    notifications->addNotification("place all ships\nto start the battle");
  }
}

void BattleShipGame::finishBattle(bool meWin) {
  fields["enemyField"]->setState(INACTIVE);
  user.status = END_OF_GAME;
  auto &ttl = titles["end"];
  if (meWin) {
    ttl->setText("you win!");
  } else {
    ttl->setText("you lose!");
  }
  ttl->setPosition(sf::Vector2<float>(
      static_cast<float>(screen.width) / 2.f - ttl->getSize().width / 2.f,
      static_cast<float>(screen.height) / 2.f - ttl->getSize().height / 2.f));

  if (meWin) {
    sf::Packet packet;
    packet << ME_WIN;
    server->send(packet);
    // TODO(RaEzhov): update rating in server.
  }
}

void BattleShipGame::changeSide() {
  user.myMove = !user.myMove;
}

void BattleShipGame::addFriend() {
  auto friendLogin = entries["friends"]->getStr();
  sf::Packet packet;
  packet << ADD_FRIEND << friendLogin;
  server->send(packet);
  // TODO (RaEzhov): clear entry.
  packet.clear();
  packet << GET_FRIENDS;
  server->send(packet);
}

void BattleShipGame::removeFriend() {
  auto friendLogin = entries["friends"]->getStr();
  sf::Packet packet;
  packet << RM_FRIEND << friendLogin;
  server->send(packet);
  // TODO (RaEzhov): clear entry.
  packet.clear();
  packet << GET_FRIENDS;
  server->send(packet);
}

void BattleShipGame::randomRival() {
  // Request to server for random rival
  sf::Packet packet;
  packet << WANT_RAND_PLAY << user.id;
  checkServer(server->send(packet));
  titles["myName"]->setColor(beige);
  titles["myLevel"]->setColor(beige);
  setEnemyTitles();
  pictures["end"]->setPosition(sf::Vector2<float>{
      static_cast<float>(screen.width) * 0.5f
          - pictures["end"]->getSize().width * 0.5f,
      static_cast<float>(screen.height) * 0.5f
          - pictures["end"]->getSize().height * 0.5f +
          static_cast<float>(screen.height) * 0.1f});
  fields["myField"]->setState(PLACEMENT);
  fields["enemyField"]->setState(ENEMY_INACTIVE);
  fields["myField"]->clearShips();
  fields["enemyField"]->clearShips();

  notifications->addNotification("waiting for enemy");
  user.status = IN_MP_MENU;
}

void BattleShipGame::checkServer(sf::Socket::Status status) {
  if (status != sf::Socket::Status::Done) {
    notifications->addNotification("lost connection\nto server");
    serverConnected = false;
  } else {
    serverConnected = true;
  }
}

void BattleShipGame::serverListener() {
  sf::Packet packet;
  while (true) {
    checkServer(server->receive(packet));
    int status;
    packet >> status;

    // Variables
    // for GET_FRIENDS
    unsigned int size;
    std::string frnd;
    // for ENEMY_FOUND
    std::string enemyLogin;
    unsigned int enemyId, enemyRating;

    switch (status) {
      case GET_FRIENDS:
        packet >> size;
        pages["friends"]->clearTitles();
        for (int i = 0; i < size; i++) {
          packet >> frnd;
          pages["friends"]->addTitle(frnd,
                                     [this, frnd] { multiPlayerFunc(frnd); });
        }
        break;
      case ENEMY_FOUND:
        packet >> enemyLogin >> enemyId >> enemyRating >> user.myMove;
        enemy.init(enemyLogin, enemyId, enemyRating);
        setEnemyTitles();
        // does not waits me
        enemy.wait = false;
        break;
      case ENEMY_FIELD: {
        sf::Uint16 temp;
        const char sizes[10] = {1, 1, 1, 1, 2, 2, 2, 3, 3, 4};
        fields["enemyField"]->clearShips();
        fields["enemyField"]->clearAvailability();
        for (char size_ : sizes) {
          packet >> temp;
          fields["enemyField"]->addShip((temp & 960) >> 6, (temp & 60) >> 2,
                                        size_);
          switch (size_) {
            case 1:(--(fields["enemyField"]->ship1.end()))->updateAvailability(
                &fields["enemyField"]->cells);
              break;
            case 2:(--(fields["enemyField"]->ship2.end()))->updateAvailability(
                &fields["enemyField"]->cells);
              break;
            case 3:(--(fields["enemyField"]->ship3.end()))->updateAvailability(
                &fields["enemyField"]->cells);
              break;
            case 4:(--(fields["enemyField"]->ship4.end()))->updateAvailability(
                &fields["enemyField"]->cells);
              break;
          }
        }
        fields["enemyField"]->clearAvailability(true);
        enemy.wait = true;
        break;
      }
      case DO_MOVE: {
        std::pair<unsigned char, unsigned char> move;
        packet >> move.first >> move.second;
        fields["myField"]->cells[move.first][move.second].shoot();
        if (fields["myField"]->cells[move.first][move.second].isUnderShip()) {
          fields["myField"]->findShip(move);
        }
        changeSide();
        break;
      }
      case ENEMY_DISCONNECTED:
        notifications->addNotification("enemy\ndisconnected");
        mainMenu();
        break;
      case UPD_RATING:
        packet >> user.rating;
        break;
      case WANT_FRIEND_PLAY:
        packet >> frnd;
        notifications->addNotification(frnd + " wants to\nplay with you!");
    }
  }
}
