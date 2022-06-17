// Copyright 2022 Roman Ezhov. Github: RaEzhov

#ifndef SERVER_DB_CONNECTION_H_
#define SERVER_DB_CONNECTION_H_

#include <pqxx/pqxx>

#include <memory>
#include <utility>
#include <list>
#include <string>

#include <SFML/Network.hpp>

#include "server/config.h"

using IdRating = std::pair<unsigned int, unsigned int>;

enum UserStatus {
  OFFLINE = 0,
  ONLINE = 1
};

class DBConnection {
 public:
  DBConnection();

  ~DBConnection();

  std::string getLogin(unsigned int id);

  std::unordered_set<unsigned int> getFriends(unsigned int id);

  void addFriend(unsigned int usr, unsigned int frnd);

  bool isPasswordCorrect(const std::string &login, const std::string &password);

  bool isUserRegistered(const std::string &login, const std::string &password);

  bool isUserOnline(unsigned int id);

  void updateStatus(unsigned int id, UserStatus status);

  IdRating getUserIdRating(const std::string &login);

  static bool correctLoginOrPassword(const std::string &str);

 private:
  std::string connectionString;
  std::unique_ptr<pqxx::connection> conn;
  std::unique_ptr<pqxx::work> w;
  sf::Mutex m;
};

#endif  // SERVER_DB_CONNECTION_H_
