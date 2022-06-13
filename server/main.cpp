// Copyright 2022 Roman Ezhov. Github: RaEzhov

#include <thread>
#include <list>
#include <unordered_map>
#include <csignal>
#include <queue>

#include <SFML/Network.hpp>

#include "server/db_connection.h"
#include "server/logger.h"

#include "global/message_status.h"

const int PORT = 12345;

static std::unique_ptr<sf::TcpListener> listener;

static std::unique_ptr<DBConnection> conn;

static std::list<std::unique_ptr<sf::TcpSocket>> clients;

static std::unordered_map<unsigned int,
                          std::list<std::unique_ptr<sf::TcpSocket>>::iterator>
    clientsMap;

static std::queue<unsigned int> randPlayQueue;

void clientLoop(std::list<std::unique_ptr<sf::TcpSocket>>::iterator client,
                unsigned int id) {
  clientsMap[id] = client;
  sf::Packet packet;
  int status;
  conn->updateStatus(id, ONLINE);
  std::string msg;
  sf::Socket::Status connected = sf::Socket::Status::Done;
  connected = (*client)->receive(packet);
  while (connected == sf::Socket::Status::Done) {
    packet >> status;

    std::list<unsigned int> friends;

    switch (status) {
      case GET_FRIENDS:
        friends = conn->getFriends(id);
        packet.clear();
        packet << GET_FRIENDS << static_cast<unsigned int>(friends.size());
        for (auto &f : friends) {
          packet << conn->getLogin(f);
        }
        (*client)->send(packet);
        Logger::log("user " + std::to_string(id) + " get friends");
        break;
      case ADD_FRIEND:
        break;
      case DO_MOVE: {
        sf::Packet packet2;
        unsigned int enemyId;
        std::pair<unsigned char, unsigned char> move;
        packet >> enemyId;
        packet >> move.first >> move.second;
        packet.clear();
        packet << DO_MOVE << move.first << move.second;
        if (clientsMap.find(enemyId) != clientsMap.end()) {
          (*(clientsMap[enemyId]))->send(packet);
        } else {
          packet.clear();
          packet << ENEMY_DISCONNECTED;
          (*client)->send(packet);
        }
        break;
      }
      case WANT_RAND_PLAY:
        if (randPlayQueue.empty()) {
          randPlayQueue.push(id);
          Logger::log("User " + std::to_string(id) + " added to queue");
        } else {
          // Get enemy
          auto enemyId = randPlayQueue.front();
          randPlayQueue.pop();

          // Send info to this user
          auto enemyLogin = conn->getLogin(enemyId);
          auto enemyIdRating = conn->getUserIdRating(enemyLogin);
          packet.clear();
          packet << ENEMY_FOUND << enemyLogin << enemyIdRating.first
                 << enemyIdRating.second
                 << true;  // he moves first
          (*client)->send(packet);

          // Send info to enemy
          auto myLogin = conn->getLogin(id);
          auto myIdRating = conn->getUserIdRating(myLogin);
          packet.clear();
          packet << ENEMY_FOUND << myLogin << myIdRating.first
                 << myIdRating.second
                 << false;  // he moves second
          if (clientsMap.find(enemyId) != clientsMap.end()) {
            (*(clientsMap[enemyId]))->send(packet);
            Logger::log(
                "users " + std::to_string(id) + ' ' + std::to_string(enemyId)
                    + " went to game");
          }
        }
        break;
      case ENEMY_FIELD: {
        unsigned int enemyId;
        packet >> enemyId;
        sf::Packet packet2;
        packet2 << ENEMY_FIELD;
        sf::Uint16 temp;
        for (int i = 0; i < 10; i++) {
          packet >> temp;
          packet2 << temp;
        }
        if (clientsMap.find(enemyId) != clientsMap.end()) {
          (*(clientsMap[enemyId]))->send(packet2);
        } else {
          packet.clear();
          packet << ENEMY_DISCONNECTED;
          (*client)->send(packet);
        }
        break;
      }
      default:Logger::log("wrong message status from " + std::to_string(id));
    }
    packet.clear();
    connected = (*client)->receive(packet);
  }
  conn->updateStatus(id, OFFLINE);
  Logger::log("client " + (*client)->getRemoteAddress().toString() + ":"
                  + std::to_string((*client)->getRemotePort())
                  + " disconnected");
  clientsMap.erase(id);
  clients.erase(client);
}

void authUser(std::list<std::unique_ptr<sf::TcpSocket>>::iterator user) {
  std::string login, password;
  sf::Packet packet;
  auto connected = sf::Socket::Status::Done;
  bool isAuth = false;
  auto userIp = (*user)->getRemoteAddress();
  auto userPort = (*user)->getRemotePort();
  while (connected == sf::Socket::Status::Done && !isAuth) {
    connected = (*user)->receive(packet);
    packet >> login >> password;
    if (login[0] != '@') {
      isAuth = conn->isPasswordCorrect(login, password);
    } else {
      login.erase(login.cbegin());
      isAuth = conn->isUserRegistered(login, password);
    }
    packet.clear();
    packet << isAuth;
    (*user)->send(packet);
    packet.clear();
  }
  if (connected == sf::Socket::Status::Done) {
    Logger::log("client " + userIp.toString() + ":" + std::to_string(userPort)
                    + " has authenticated");
    auto idRating = conn->getUserIdRating(login);
    packet.clear();
    packet << idRating.first << idRating.second;
    (*user)->send(packet);
    clientLoop(user, idRating.first);
  } else {
    Logger::log("client " + userIp.toString() + ":" + std::to_string(userPort)
                    + " disconnected");
  }
}

void signalCallbackHandler(int signum) {
  listener->close();
  Logger::log("program terminated");
  exit(0);
}

int main() {
  listener = std::make_unique<sf::TcpListener>();
  signal(SIGINT, signalCallbackHandler);
  signal(SIGTERM, signalCallbackHandler);
  try {
    conn = std::make_unique<DBConnection>();
    Logger::log("database connected");
  }
  catch (const std::exception &e) {
    Logger::log(e.what(), ERROR);
    return 1;
  }

  // bind the listener to a port
  if (listener->listen(PORT) != sf::Socket::Done) {
    Logger::log("listener cannot start", ERROR);
    return 1;
  }
  Logger::log("listener started");
  while (true) {
    clients.push_back(std::make_unique<sf::TcpSocket>());
    if (listener->accept(**(--clients.end())) != sf::Socket::Done) {
      Logger::log("error accepting", WARNING);
    }
    Logger::log(
        "client " + (**(--clients.end())).getRemoteAddress().toString() +
            ":" + std::to_string((**(--clients.end())).getRemotePort())
            + " accepted");

    std::thread clientThread(authUser, --clients.end());
    clientThread.detach();
  }
}
