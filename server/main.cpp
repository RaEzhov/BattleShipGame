#include <iostream>
#include <sstream>
#include <unordered_set>
#include <pqxx/pqxx>

char CONN_STR[] = "hostaddr=127.0.0.1 port=5432 dbname=battleship_db user=roman password=roman";

class DBConnection {
public:
    DBConnection() {
        conn = new pqxx::connection(CONN_STR);
        w = new pqxx::work(*conn);
    }

    ~DBConnection() {
        w->commit();
        delete w;
        delete conn;
    }

    /** Inserts new user to table users.\n Login and password must already be checked for correctness */
    void insertNewUser(const std::string &login, const std::string &password) {
        auto id = w->exec1("SELECT MAX(id) FROM users");
        std::stringstream ss;
        ss << "INSERT INTO users VALUES ('" << login << "', '" << password << "', " << id[0].as<int>() + 1 << ", 0)";
        w->exec(ss.str());
    }

    /** Select all logins from database and insert it to userLogins set*/
    void selectUsers(std::unordered_set<std::string> &userLogins) {
        auto users = w->exec("SELECT login FROM users");
        for (auto i: users) {
            userLogins.insert(i[0].as<std::string>());
        }
    }

private:
    pqxx::connection *conn;
    pqxx::work *w;
};

int main() {
    try {
        DBConnection conn;
        std::unordered_set<std::string> set;
        conn.selectUsers(set);
        for (auto &i: set) {
            std::cout << i << ' ';
        }
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}