#include "config.h"
#include <fstream>

std::unique_ptr<Config> Config::pInstance = nullptr;

const Config &Config::instance() {
    if (!pInstance){
        pInstance.reset(new Config{});
    }
    return *pInstance;
}

Config::Config(): port(int{}) {
    std::ifstream cfg;
    cfg.open("./config");

    cfg >> port >> dbIp >> dbUser >> dbPassword;

    cfg.close();
}