#include <thread>
#include <list>
#include <unordered_set>
#include <SFML/Network.hpp>
#include <memory>
#include <csignal>

#include "db_connection.h"
#include "logger.h"

#include "../message_status.h"

using namespace sf;

const char IP_ADDR[] = "127.0.0.1";

const int PORT = 55555;

static std::unique_ptr<TcpListener> listener;

static std::unique_ptr<DBConnection> conn;

static std::list<std::unique_ptr<TcpSocket>> clients;

void clientLoop(std::list<std::unique_ptr<TcpSocket>>::iterator client, unsigned int id) {
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
                packet << static_cast<unsigned int>(friends.size());
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
            default:
                Logger::log("wrong message status from " + std::to_string(id));
        }
        packet.clear();
    }
    conn->updateStatus(id, OFFLINE);
    Logger::log("client " + (*client)->getRemoteAddress().toString() + ":" + std::to_string((*client)->getRemotePort()) + " disconnected");
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
