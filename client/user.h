// Copyright 2022 Roman Ezhov. Github: RaEzhov

#ifndef CLIENT_USER_H_
#define CLIENT_USER_H_

#include <iostream>
#include <string>
#include <utility>

enum UserStatus {
  LOGIN,
  MAIN_MENU,
  GET,
  IN_SP_MENU,
  IN_SP_GAME,
  IN_MP_MENU,
  IN_MP_GAME,
  END_OF_GAME,
  IN_MP_LOBBY
};

class User {
 public:
  friend class BattleShipGame;

  User();

  std::string getRatingStr() const;

  std::string getNameStr() const;

  void init(std::string login_, unsigned int id_, unsigned int rating_);

  void reset();

 private:
  std::string login;
  unsigned int id;
  unsigned int rating;
  UserStatus status;
  bool myMove;
  bool wait;
};

#endif  // CLIENT_USER_H_
