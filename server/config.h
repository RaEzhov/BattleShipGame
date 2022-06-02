#ifndef CONFIG_H
#define CONFIG_H
#include <memory>
#include <string>
#include <fstream>

class Config {
public:

    static const Config &instance();

    std::string dbIp, dbUser, dbPassword;
    int port;
private:

    Config();

    static std::unique_ptr<Config> pInstance;
};

#endif//CONFIG_H
