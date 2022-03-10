#include <iostream>
#include <thread>
#include <list>
#include <unordered_set>
#include <pqxx/pqxx>
#include <SFML/Network.hpp>

using namespace sf;

const char IP_ADDR[] = "127.0.0.1";

const int PORT = 55555;

char CONN_STR[] = "hostaddr=127.0.0.1 port=5432 dbname=battleship_db user=roman password=roman";

class DBConnection {
public:
    DBConnection() {
        conn = new pqxx::connection(CONN_STR);
        w = new pqxx::work(*conn);
    }

    ~DBConnection() {
        w->commit();
        delete w;
        delete conn;
    }

    /** Inserts new user to table users.\n Login and password must already be checked for correctness */
    void insertNewUser(const std::string &login, const std::string &password) {
        Mutex m;
        std::stringstream ss;
        m.lock();
        auto id = w->exec1("SELECT MAX(id) FROM users");
        ss << "INSERT INTO users VALUES ('" << login << "', '" << password << "', " << id[0].as<int>() + 1 << ", 0)";
        w->exec(ss.str());
        m.unlock();
    }

    /** Select all logins from database and insert it to userLogins set*/
    void selectUsers(std::unordered_set<std::string> &userLogins) {
        Mutex m;
        m.lock();
        auto users = w->exec("SELECT login FROM users");
        m.unlock();
        for (auto i: users) {
            userLogins.insert(i[0].as<std::string>());
        }
    }

    /**Checks is password is correct for user*/
    bool isPasswordCorrect(const std::string &login, const std::string &password) {
        Mutex m;
        m.lock();
        auto dbPassword = w->exec("SELECT password FROM users WHERE users.login = '" + login + "';");
        m.unlock();
        if (dbPassword.empty() || dbPassword[0].empty()) {
            return false;
        }
        return dbPassword[0][0].as<std::string>() == password;
    }

private:
    pqxx::connection *conn;
    pqxx::work *w;
};

static DBConnection* conn;

static std::list<TcpSocket*> clients;

void clientLoop(std::list<TcpSocket*>::iterator client) {
    Packet packet;
    unsigned int status;
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

void authUser(std::list<TcpSocket*>::iterator user) {
    std::string login, password;
    Packet packet;
    auto connected = Socket::Status::Done;
    bool isAuth = false;
    while (connected == Socket::Status::Done && !isAuth) {
        connected = (*user)->receive(packet);
        packet >> login >> password;
        isAuth = conn->isPasswordCorrect(login, password);
        packet.clear();
        packet << isAuth;
        (*user)->send(packet);
        packet.clear();
    }
    std::cout << "Client " << (*user)->getRemoteAddress() << ":" << (*user)->getRemotePort() << " has authenticated!\n";
    clientLoop(user);
}

int main() {
    std::unordered_set<std::string> set;
    try {
        conn = new DBConnection;
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl; //log
        return 1;
    }
    std::cout << "Database connected\n";

    TcpListener listener;
    // bind the listener to a port
    if (listener.listen(PORT) != sf::Socket::Done) {
        std::cout << "Listen error!";
        return 1;
    }
    std::cout << "Listener started\n";
    while (true) {
        clients.push_back(new TcpSocket);
        if (listener.accept(**(--clients.end())) != Socket::Done) {
            std::cout << "Accept error!\n";
        }
        std::cout << "Client " << (**(--clients.end())).getRemoteAddress() << ":"
                  << (**(--clients.end())).getRemotePort() << " accepted!\n";
        std::thread clientThread(authUser, --clients.end());
        clientThread.detach();
    }
    listener.close();
    delete conn;

    return 0;
}