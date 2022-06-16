// Copyright 2022 Roman Ezhov. Github: RaEzhov

#include <fstream>

#include "server/config.h"

std::string getEnvVar(std::string const &key) {
  char *val = getenv(key.c_str());
  return val == NULL ? std::string("") : std::string(val);
}

std::unique_ptr<Config> Config::pInstance = nullptr;


const Config &Config::instance() {
  if (!pInstance) {
    pInstance.reset(new Config{});
  }
  return *pInstance;
}

Config::Config() : port{} {
  std::ifstream cfg;
  cfg.open("./config");

  if (cfg.good()) {
    cfg >> port >> dbIp >> dbPort >> dbUser >> dbPassword;
  } else {
    port = std::atoi(getEnvVar("BATTLESHIP_SERVER_PORT").c_str());
    dbIp = getEnvVar("BATTLESHIP_SERVER_DBIP");
    dbPort = getEnvVar("BATTLESHIP_SERVER_DBPORT");
    dbUser = getEnvVar("BATTLESHIP_SERVER_DBUSER");
    dbPassword = getEnvVar("BATTLESHIP_SERVER_DBPASSWORD");
  }
  cfg.close();
}
