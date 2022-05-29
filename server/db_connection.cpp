#include "db_connection.h"
#include "logger.h"

std::string DBConnection::CONN_STR = std::move(std::string("hostaddr=" + Config::instance().dbIp + " port=5432 dbname=battleship_db user=" +
        Config::instance().dbUser + " password=" + Config::instance().dbPassword));

bool DBConnection::isPasswordCorrect(const std::string &login, const std::string &password) {
    sf::Mutex m;
    m.lock();
    auto dbPassword = w->exec("SELECT password, status FROM users WHERE users.login = '" + login + "';");
    m.unlock();
    if (dbPassword.empty() || dbPassword[0].empty() || dbPassword[0][1].as<unsigned int>() != OFFLINE) {
        return false;
    }
    return dbPassword[0][0].as<std::string>() == password;
}

bool DBConnection::isUserRegistered(const std::string &login, const std::string &password) {
    if (login.size() < 4 || password.size() < 4) {
        return false;
    }
    sf::Mutex m;
    m.lock();
    auto countUsers =
            w->exec1("SELECT count(users.login) FROM users WHERE login = '" + login + "';");
    auto maxId = (w->exec1("SELECT max(id) FROM users;"))[0].as<int>();
    if (countUsers[0].as<int>() == 0) {
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

DBConnection::~DBConnection() {
    w->commit();
}

DBConnection::DBConnection() : conn(std::make_unique<pqxx::connection>(CONN_STR)), w(std::make_unique<pqxx::work>(*conn)){
    Logger::log("database connection string is " + CONN_STR);
}

void DBConnection::updateStatus(unsigned int id, UserStatus status) {
    sf::Mutex m;
    m.lock();
    w->exec("UPDATE  users SET status = " + std::to_string(status) + " WHERE id = " + std::to_string(id) + ";");
    m.unlock();
}

std::string DBConnection::getLogin(unsigned int id) {
    sf::Mutex m;
    m.lock();
    auto result = w->exec1("SELECT login FROM users WHERE id = " + std::to_string(id) + ";");
    m.unlock();
    return result[0].as<std::string>();
}

std::list<unsigned int> DBConnection::getFriends(unsigned int id) {
    sf::Mutex m;
    m.lock();
    auto result = w->exec("SELECT user_id FROM friends WHERE friend_id = " + std::to_string(id) + ";");
    m.unlock();
    std::list<unsigned int> friends;
    for (auto row: result){
        for (auto i: row){
            friends.push_back(i.as<unsigned int>());
        }
    }
    return std::move(friends);
}

void DBConnection::addFriend(unsigned int usrId, unsigned int frndId) {
    sf::Mutex m;
    m.lock();
    auto result = w->exec1(
            "SELECT count(user_id) from friends WHERE user_id = " +
            std::to_string(usrId) + " AND friend_id = " + std::to_string(frndId) + ";");
    if (result[0].as<unsigned int>() == 1){
        return;
    }
    w->exec("INSERT INTO friends (user_id, friend_id) VALUES (" + std::to_string(usrId) + ", " + std::to_string(frndId) + ");");
    m.unlock();
}
