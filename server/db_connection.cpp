#include "db_connection.h"

char DBConnection::CONN_STR[] = "hostaddr=127.0.0.1 port=5432 dbname=battleship_db user=roman password=roman";

void DBConnection::insertNewUser(const std::string &login, const std::string &password) {
    sf::Mutex m;
    std::stringstream ss;
    m.lock();
    auto id = w->exec1("SELECT MAX(id) FROM users");
    ss << "INSERT INTO users VALUES ('" << login << "', '" << password << "', " << id[0].as<int>() + 1 << ", 0)";
    w->exec(ss.str());
    m.unlock();
}

void DBConnection::selectUsers(std::unordered_set<std::string> &userLogins) {
    sf::Mutex m;
    m.lock();
    auto users = w->exec("SELECT login FROM users");
    m.unlock();
    for (auto i: users) {
        userLogins.insert(i[0].as<std::string>());
    }
}

bool DBConnection::isPasswordCorrect(const std::string &login, const std::string &password) {
    sf::Mutex m;
    m.lock();
    auto dbPassword = w->exec("SELECT password FROM users WHERE users.login = '" + login + "';");
    m.unlock();
    if (dbPassword.empty() || dbPassword[0].empty()) {
        return false;
    }
    return dbPassword[0][0].as<std::string>() == password;
}