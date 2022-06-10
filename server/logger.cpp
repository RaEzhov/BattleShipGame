// Copyright 2022 Roman Ezhov. Github: RaEzhov

#include <iostream>
#include <chrono>
#include <iomanip>
#include <string>

#include "server/logger.h"

std::unique_ptr<Logger> Logger::pLogger = nullptr;

Logger::Logger() {
  // Open file in append mode
  output.open("./log.txt", std::ios_base::out | std::ios_base::app);
}

Logger::~Logger() {
  Logger::log("logger closed");
  output.close();
}

void Logger::log(const std::string &log, LogType type) {
  if (!pLogger) {
    pLogger.reset(new Logger{});
    Logger::log("logger opened");
  }
  std::stringstream logStream;
  auto time = std::chrono::system_clock::now();
  const std::time_t t_c = std::chrono::system_clock::to_time_t(time);
  logStream << std::put_time(std::localtime(&t_c), "%F %T: ")
            << log << std::endl;
  switch (type) {
    case INFO:std::cout << "\033[0;37m" << logStream.str() << "\033[0m";
      break;
    case WARNING:std::cout << "\033[0;33m" << logStream.str() << "\033[0m";
      break;
    case ERROR:std::cout << "\033[0;31m" << logStream.str() << "\033[0m";
      break;
  }
  pLogger->output << logStream.str();
}
