// Copyright 2022 Roman Ezhov. Github: RaEzhov

#include "client/user.h"

User::User():login("-"), id(0), rating(0), status(LOGIN),
             wait(false), myMove(true) {}

std::string User::getRatingStr() const {
  return "level: " + std::to_string(rating / 100);
}

void User::init(std::string  login_, unsigned int id_, unsigned int rating_) {
  login = std::move(login_);
  id = id_;
  rating = rating_;
  status = LOGIN;
  wait = myMove = true;
}

std::string User::getNameStr() const {
  return login;
}

void User::reset() {
  login = "-";
  id = 0;
  rating = 0;
}
