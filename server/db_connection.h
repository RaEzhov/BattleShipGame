#ifndef DB_CONNECTION_H
#define DB_CONNECTION_H

#include <pqxx/pqxx>
#include <SFML/Network.hpp>

#include "config.h"

enum UserStatus{
    OFFLINE = 0,
    ONLINE = 1,

};


class DBConnection {
public:
    DBConnection();

    ~DBConnection();

    std::string getLogin(unsigned int id);

    std::list<unsigned int> getFriends(unsigned int id);

    void addFriend(unsigned int usr, unsigned int frnd);

    /**Checks if password is correct for user*/
    bool isPasswordCorrect(const std::string &login, const std::string &password);

    /**Insert new user*/
    bool isUserRegistered(const std::string &login, const std::string &password);

    void updateStatus(unsigned int id, UserStatus status);

    std::pair<unsigned int, unsigned int> getUserIdRating(const std::string &login);

private:
    std::unique_ptr<pqxx::connection> conn;
    std::unique_ptr<pqxx::work> w;
    static std::string CONN_STR;
};

#endif//DB_CONNECTION_H
