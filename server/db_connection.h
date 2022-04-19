#ifndef DB_CONNECTION_H
#define DB_CONNECTION_H

#include <pqxx/pqxx>
#include <unordered_set>
#include <SFML/Network.hpp>


class DBConnection {
public:
    DBConnection() : conn(std::make_unique<pqxx::connection>(CONN_STR)), w(std::make_unique<pqxx::work>(*conn)){}

    ~DBConnection() { w->commit(); }

    /** Inserts new user to table users.\n Login and password must already be checked for correctness */
    void insertNewUser(const std::string &login, const std::string &password);

    /** Select all logins from database and insert it to userLogins set*/
    void selectUsers(std::unordered_set<std::string> &userLogins);

    /**Checks if password is correct for user*/
    bool isPasswordCorrect(const std::string &login, const std::string &password);

    /**Insert new user*/
    bool isUserRegistered(const std::string &login, const std::string &password);

private:
    std::unique_ptr<pqxx::connection> conn;
    std::unique_ptr<pqxx::work> w;
    static char CONN_STR[];
};

#endif//DB_CONNECTION_H
