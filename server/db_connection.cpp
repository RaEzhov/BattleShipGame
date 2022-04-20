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

bool DBConnection::isUserRegistered(const std::string &login, const std::string &password) {
    if (login.size() < 4 || password.size() < 4){
        return false;
    }
    sf::Mutex m;
    m.lock();
    auto countUsers =
            w->exec1("SELECT count(users.login) FROM users WHERE login = '" + login + "';");
    auto maxId = (w->exec1("SELECT max(id) FROM users;"))[0].as<int>();
    if (countUsers[0].as<int>() == 0){
        w->exec("INSERT INTO users VALUES ('" + login + "', '" + password + "', " + std::to_string(maxId + 1) + ", 0);");
        m.unlock();
        return true;
    }
    m.unlock();
    return false;
}

std::pair<unsigned int, unsigned int> DBConnection::getUserIdRating(const std::string &login) {
    sf::Mutex m;
    m.lock();
    auto idRating = w->exec("SELECT id, rating FROM users WHERE users.login = '" + login + "';");
    m.unlock();
    if (idRating.empty() || idRating[0].empty()) {
        return {0, 0};
    }
    return {idRating[0][0].as<unsigned int>(), idRating[0][1].as<unsigned int>()};
}
