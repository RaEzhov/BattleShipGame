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
    IN_MP_GAME,
    END_OF_GAME
};

class User {
public:
    friend class BattleShipGame;

    User();

    std::string getRatingStr() const;

    void init(std::string  login_, unsigned int id_, unsigned int rating_);

private:
    std::string login;
    unsigned int id;
    unsigned int rating;
    UserStatus status;
    bool myMove;
    bool wait;
};

#endif//USER_H
