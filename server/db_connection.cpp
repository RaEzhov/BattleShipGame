// Copyright 2022 Roman Ezhov. Github: RaEzhov

#include <memory>
#include <utility>
#include <list>
#include <string>
#include <unordered_set>

#include "server/db_connection.h"
#include "server/logger.h"

bool DBConnection::correctLoginOrPassword(const std::string &str) {
  return str.size() >= 4 &&
  str.find_first_not_of("abcdefghijklmnopqrstuvwxyz0123456789") ==
  std::string::npos;
}

bool DBConnection::isPasswordCorrect(const std::string &login,
                                     const std::string &password) {
  if (!correctLoginOrPassword(login) || !correctLoginOrPassword(password)) {
    return false;
  }
  m.lock();
  auto dbPassword = w->exec(
      "SELECT password, status FROM users WHERE users.login = '" + login
          + "' ;");
  m.unlock();
  if (dbPassword.empty() || dbPassword[0].empty() ||
  dbPassword[0][1].as<unsigned int>() != OFFLINE) {
    return false;
  }
  return dbPassword[0][0].as<std::string>() == password;
}

bool DBConnection::isUserRegistered(const std::string &login,
                                    const std::string &password) {
  if (!correctLoginOrPassword(login) || !correctLoginOrPassword(password)) {
    return false;
  }
  m.lock();
  auto countUsers =
      w->exec1("SELECT count(users.login) FROM users WHERE login = '" + login
                   + "';");
  auto maxId = (w->exec1("SELECT max(id) FROM users;"))[0].as<int>();
  if (countUsers[0].as<int>() == 0) {
    w->exec("INSERT INTO users VALUES ('" + login + "', '" + password + "', "
                + std::to_string(maxId + 1) + ", 0) ;");
    m.unlock();
    return true;
  }
  m.unlock();
  return false;
}

IdRating DBConnection::getUserIdRating(const std::string &login) {
  if (!correctLoginOrPassword(login)) {
    return {0, 0};
  }
  m.lock();
  auto idRating = w->exec(
      "SELECT id, rating FROM users WHERE users.login = '" + login + "' ;");
  m.unlock();
  if (idRating.empty() || idRating[0].empty()) {
    return {0, 0};
  }
  return {idRating[0][0].as<unsigned int>(), idRating[0][1].as<unsigned int>()};
}

DBConnection::~DBConnection() {
  w->commit();
}

DBConnection::DBConnection()
    : connectionString(
        std::string(
            "hostaddr=" + Config::instance().dbIp +
            " port=" + Config::instance().dbPort +
            " dbname=battleship_db user=" + Config::instance().dbUser +
            " password=" + Config::instance().dbPassword)),
      conn(std::make_unique<pqxx::connection>(connectionString)),
      w(std::make_unique<pqxx::work>(*conn)), m{} {
  Logger::log("database connection string is " + connectionString);
}

void DBConnection::updateStatus(unsigned int id, UserStatus status) {
  m.lock();
  w->exec(
      "UPDATE  users SET status = " + std::to_string(status) + " WHERE id = "
          + std::to_string(id) + " ;");
  m.unlock();
}

std::string DBConnection::getLogin(unsigned int id) {
  m.lock();
  auto result = w->exec1(
      "SELECT login FROM users WHERE id = " + std::to_string(id) + " ;");
  m.unlock();
  return result[0].as<std::string>();
}

std::unordered_set<unsigned int> DBConnection::getFriends(unsigned int id) {
  m.lock();
  auto result = w->exec(
      "SELECT user_id FROM friends WHERE friend_id = " +
      std::to_string(id) + " ;");
  m.unlock();
  std::unordered_set<unsigned int> friends;
  for (auto row : result) {
    for (auto i : row) {
      friends.insert(i.as<unsigned int>());
    }
  }

  m.lock();
  result = w->exec(
      "SELECT friend_id FROM friends WHERE user_id = " +
          std::to_string(id) + " ;");
  m.unlock();
  for (auto row : result) {
    for (auto i : row) {
      friends.insert(i.as<unsigned int>());
    }
  }
  return std::move(friends);
}

void DBConnection::addFriend(unsigned int usrId, unsigned int frndId) {
  m.lock();
  auto result = w->exec1(
      "SELECT count(user_id) FROM friends WHERE user_id = "
      + std::to_string(usrId)
      + " AND friend_id = " + std::to_string(frndId) + " ;");
  if (result[0].as<unsigned int>() == 1) {
    return;
  }
  w->exec("INSERT INTO friends (user_id, friend_id) VALUES ("
              + std::to_string(usrId) + ", " + std::to_string(frndId) + ") ;");
  m.unlock();
}
bool DBConnection::isUserOnline(unsigned int id) {
  m.lock();
  auto result = w->exec1("SELECT status FROM users WHERE id = " +
      std::to_string(id) + " ;");
  return result[0].as<int>() != 0;
}
void DBConnection::removeFriend(unsigned int usr, unsigned int frnd) {
  m.lock();
  w->exec("DELETE FROM friends WHERE user_id = " + std::to_string(usr) +
  " AND friend_id = " + std::to_string(frnd) + " OR user_id = " +
  std::to_string(frnd) + " AND friend_id = " + std::to_string(usr) + ";");
}
