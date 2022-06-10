// Copyright 2022 Roman Ezhov. Github: RaEzhov

#include <fstream>

#include "client/config.h"

std::unique_ptr<Config> Config::pInstance = nullptr;

const Config &Config::instance() {
  if (!pInstance) {
    pInstance.reset(new Config{});
  }
  return *pInstance;
}

Config::Config(): port(int{}), buttonScale(float{}), framerate(60) {
  std::ifstream cfg;
  cfg.open("config");

  cfg >> ip >> port >> resources >> framerate >> buttonScale;

  cfg.close();
}
