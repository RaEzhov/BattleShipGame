// Copyright 2022 Roman Ezhov. Github: RaEzhov

#ifndef SERVER_CONFIG_H_
#define SERVER_CONFIG_H_

#include <memory>
#include <string>
#include <fstream>

class Config {
 public:
  static const Config &instance();

  std::string dbIp, dbPort, dbUser, dbPassword;
  int port;

 private:
  Config();

  static std::unique_ptr<Config> pInstance;
};

#endif  // SERVER_CONFIG_H_
