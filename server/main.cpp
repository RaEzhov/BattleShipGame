#include <thread>
#include <list>
#include <unordered_set>
#include <SFML/Network.hpp>
#include <memory>
#include <csignal>
#include <queue>

#include "db_connection.h"
#include "logger.h"

#include "../message_status.h"

using namespace sf;

const char IP_ADDR[] = "127.0.0.1";

const int PORT = 55555;

static std::unique_ptr<TcpListener> listener;

static std::unique_ptr<DBConnection> conn;

static std::list<std::unique_ptr<TcpSocket>> clients;

static std::unordered_map<unsigned int, std::list<std::unique_ptr<TcpSocket>>::iterator> clientsMap;

static std::queue<unsigned int> randPlayQueue;

void clientLoop(std::list<std::unique_ptr<TcpSocket>>::iterator client, unsigned int id) {
    clientsMap[id] = client;
    Packet packet;
    int status;
    conn->updateStatus(id, ONLINE);
    std::string msg;
    Socket::Status connected = Socket::Status::Done;
    while (connected == Socket::Status::Done) {
        connected = (*client)->receive(packet);
        packet >> status;

        std::list<unsigned int> friends;

        switch (status) {
            case GET_FRIENDS:
                friends = conn->getFriends(id);
                packet.clear();
                packet << GET_FRIENDS << static_cast<unsigned int>(friends.size());
                for (auto& f: friends){
                    packet << conn->getLogin(f);
                }
                (*client)->send(packet);
                Logger::log("user " + std::to_string(id) + " get friends");
                break;
            case ADD_FRIEND:
                break;
            case DO_MOVE:
                break;
            case WANT_RAND_PLAY:
                if (randPlayQueue.empty()){
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
                    packet << ENEMY_FOUND << enemyLogin << enemyIdRating.first << enemyIdRating.second << true;  // he moves first
                    (*client)->send(packet);

                    // Send info to enemy
                    auto myLogin = conn->getLogin(id);
                    auto myIdRating = conn->getUserIdRating(myLogin);
                    packet.clear();
                    packet << ENEMY_FOUND << myLogin << myIdRating.first << myIdRating.second << false;  // he moves second
                    (*(clientsMap[enemyId]))->send(packet);
                    Logger::log("users " + std::to_string(id) + ' ' + std::to_string(enemyId) + " went to game");
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
                (*(clientsMap[enemyId]))->send(packet2);
                break;
            }
            default:
                Logger::log("wrong message status from " + std::to_string(id));
        }
        packet.clear();
    }
    conn->updateStatus(id, OFFLINE);
    Logger::log("client " + (*client)->getRemoteAddress().toString() + ":" + std::to_string((*client)->getRemotePort()) + " disconnected");
    clientsMap.erase(id);
    clients.erase(client);
}

void authUser(std::list<std::unique_ptr<TcpSocket>>::iterator user) {
    std::string login, password;
    Packet packet;
    auto connected = Socket::Status::Done;
    bool isAuth = false;
    auto userIp = (*user)->getRemoteAddress();
    auto userPort = (*user)->getRemotePort();
    while (connected == Socket::Status::Done && !isAuth) {
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
    if (connected == Socket::Status::Done) {
        Logger::log("client " + userIp.toString() + ":" + std::to_string(userPort) + " has authenticated");
        auto idRating = conn->getUserIdRating(login);
        packet.clear();
        packet << idRating.first << idRating.second;
        (*user)->send(packet);
        clientLoop(user, idRating.first);
    } else {
        Logger::log("client " + userIp.toString() + ":" + std::to_string(userPort) + " disconnected");
    }
}


void signalCallbackHandler(int signum) {
    listener->close();
    Logger::log("program terminated");
    exit(0);
}

int main() {
    listener = std::make_unique<TcpListener>();
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
        clients.push_back(std::make_unique<TcpSocket>());
        if (listener->accept(**(--clients.end())) != Socket::Done) {
            Logger::log("error accepting", WARNING);
        }
        Logger::log("client " + (**(--clients.end())).getRemoteAddress().toString() +
        ":" + std::to_string((**(--clients.end())).getRemotePort()) + " accepted");

        std::thread clientThread(authUser, --clients.end());
        clientThread.detach();
    }
}
