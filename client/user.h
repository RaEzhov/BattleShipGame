#ifndef USER_H
#define USER_H
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
    IN_MP_GAME
};

class User {
public:
    friend class BattleShipGame;

    User():login("no-login"), id(0), rating(0), status(LOGIN){}

    std::string getRatingStr() const{
        return "level: " + std::to_string(rating % 100);
    }

    void init(std::string  login_, unsigned int id_, unsigned int rating_){
        login = std::move(login_);
        id = id_;
        rating = rating_;
        status = LOGIN;
    }
private:
    std::string login;
    unsigned int id;
    unsigned int rating;
    UserStatus status;
};

#endif//USER_H
