#include "config.h"
#include <fstream>

std::unique_ptr<Config> Config::pInstance = nullptr;

const Config &Config::instance() {
    if (!pInstance){
        pInstance.reset(new Config{});
    }
    return *pInstance;
}

Config::Config() {
    std::ifstream cfg;
    cfg.open("../../client/config");

    cfg >> ip >> port >> resources >> framerate >> buttonScale;

    cfg.close();
}
