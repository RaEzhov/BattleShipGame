#include <iostream>
#include <thread>
#include <list>
#include <unordered_set>
#include <SFML/Network.hpp>
#include <memory>
#include <csignal>

#include "db_connection.h"

using namespace sf;

const char IP_ADDR[] = "127.0.0.1";

const int PORT = 55555;

TcpListener listener;

static std::unique_ptr<DBConnection> conn;

static std::list<std::unique_ptr<TcpSocket>> clients;

void clientLoop(std::list<std::unique_ptr<TcpSocket>>::iterator client) {
    Packet packet;
    unsigned short status;
    std::string msg;
    Socket::Status connected = Socket::Status::Done;
    while (connected == Socket::Status::Done) {
        connected = (*client)->receive(packet);
        packet >> status >> msg;
        std::cout << "Client " << (*client)->getRemoteAddress() << ":" << (*client)->getRemotePort() << " " << status << " " << msg << "\n";
        packet.clear();
    }
    std::cout << "Client " << (*client)->getRemoteAddress() << ":" << (*client)->getRemotePort() << " disconnected!\n";
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
        std::cout << "Client " << userIp << ":" << userPort<< " has authenticated!\n";
        auto idRating = conn->getUserIdRating(login);
        std::cout << idRating.first << idRating.second << '\n';
        packet.clear();
        packet << idRating.first << idRating.second;
        (*user)->send(packet);
        clientLoop(user);
    } else {
        std::cout << "Client " << userIp << ":" << userPort << " disconnected!\n";
    }
}


void signalCallbackHandler(int signum) {
    listener.close();
    std::cout << "Program terminated\n";
    exit(signum);
}

int main() {
    signal(SIGINT, signalCallbackHandler);
    try {
        conn = std::make_unique<DBConnection>();
        std::cout << "Database connected\n";
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl; //log
        return 1;
    }

    // bind the listener to a port
    if (listener.listen(PORT) != sf::Socket::Done) {
        std::cout << "Listen error!";
        return 1;
    }
    std::cout << "Listener started\n";
    while (true) {
        clients.push_back(std::make_unique<TcpSocket>());
        if (listener.accept(**(--clients.end())) != Socket::Done) {
            std::cout << "Accept error!\n";
        }
        std::cout << "Client " << (**(--clients.end())).getRemoteAddress() << ":"
                  << (**(--clients.end())).getRemotePort() << " accepted!\n";
        std::thread clientThread(authUser, --clients.end());
        clientThread.detach();
    }
}
