#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <memory>

enum LogType{
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    static void log(const std::string& log, LogType type = INFO);
    ~Logger();
private:
    Logger();
    std::ofstream output;
    static std::unique_ptr<Logger> pLogger;
};


#endif//LOGGER_H
