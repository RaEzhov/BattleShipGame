// Copyright 2022 Roman Ezhov. Github: RaEzhov

#ifndef SERVER_LOGGER_H_
#define SERVER_LOGGER_H_

#include <fstream>
#include <memory>
#include <string>

enum LogType {
  INFO,
  WARNING,
  ERROR
};

class Logger {
 public:
  static void log(const std::string &log, LogType type = INFO);
  ~Logger();
 private:
  Logger();
  std::ofstream output;
  static std::unique_ptr<Logger> pLogger;
};

#endif  // SERVER_LOGGER_H_
