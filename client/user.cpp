#include "user.h"

User::User():login("no-login"), id(0), rating(0), status(LOGIN),
             wait(false), myMove(true){}

std::string User::getRatingStr() const {
    return "level: " + std::to_string(rating / 100);
}

void User::init(std::string  login_, unsigned int id_, unsigned int rating_){
    login = std::move(login_);
    id = id_;
    rating = rating_;
    status = LOGIN;
    wait = myMove = true;
}
