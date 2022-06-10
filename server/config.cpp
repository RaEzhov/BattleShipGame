// Copyright 2022 Roman Ezhov. Github: RaEzhov

#include <fstream>

#include "server/config.h"

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

  cfg >> port >> dbIp >> dbUser >> dbPassword;

  cfg.close();
}
