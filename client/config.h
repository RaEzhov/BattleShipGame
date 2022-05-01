#ifndef CONFIG_H
#define CONFIG_H
#include <memory>
#include <string>
#include <fstream>

class Config {
public:

    static const Config& instance();


    std::string ip, resources;
    int port;
    float buttonScale;
    unsigned int framerate;
private:

    Config();

    static std::unique_ptr<Config> pInstance;
};

#endif//CONFIG_H
