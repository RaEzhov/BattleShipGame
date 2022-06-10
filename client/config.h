// Copyright 2022 Roman Ezhov. Github: RaEzhov

#ifndef CLIENT_CONFIG_H_
#define CLIENT_CONFIG_H_

#include <memory>
#include <string>
#include <fstream>

class Config {
 public:
  static const Config &instance();

  std::string ip, resources;
  int port;
  float buttonScale;
  unsigned int framerate;
 private:
  Config();

  static std::unique_ptr<Config> pInstance;
};

#endif  // CLIENT_CONFIG_H_
