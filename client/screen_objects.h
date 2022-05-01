#ifndef SCREEN_OBJECTS_H
#define SCREEN_OBJECTS_H

#include <iostream>
#include <fstream>
#include <random>
#include <thread>
#include <chrono>
#include <list>
#include <utility>
#include <memory>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include "config.h"


enum GameFieldState{
    PLACEMENT,
    INACTIVE,
    GAME
};

enum ShipState {
    ALIVE,
    INJURED,
    DESTROYED
};

enum ShipDirection{
    UP,
    RIGHT,
    DOWN,
    LEFT
};

enum ShipCheckStatus{
    REMOVED,
    ROTATED,
    NONE
};

class ScreenObject {
public:
    explicit ScreenObject(std::shared_ptr<sf::RenderWindow> &window_) : window(window_) {}

protected:
    std::shared_ptr<sf::RenderWindow> window;
};

class Button : public ScreenObject {
public:
    Button(float x, float y, sf::Vector2<float> scale_, std::function<void()> funcRef,
           std::shared_ptr<sf::RenderWindow> window_, const std::string &text_, unsigned int textSize, sf::Color textColor_,
           const std::string &font = "Upheavtt.ttf", const std::string &buttonOn = Config::instance().resources + "button1.png",
           const std::string &buttonOff = Config::instance().resources + "button2.png");

    void draw();

    void eventCheck(sf::Event &event);

private:
    sf::Text text;
    sf::Font textFont;
    sf::Color textColor;
    sf::Vector2<float> textPosition;
    sf::Texture textureButtonOn, textureButtonOff;
    sf::Sprite spriteButtonOn, spriteButtonOff;
    bool lockClick;
    bool pressed;
    sf::Vector2<float> buttonPosition;
    sf::Vector2<float> scale;

    std::function<void()> function;
};

class Entry : public ScreenObject {
public:
    Entry(sf::Vector2<float> position, unsigned int size, std::shared_ptr<sf::RenderWindow> window_, unsigned int fontSize,
          bool isLogOrPass = true, std::function<void()> enterFunc = nullptr, const std::string& str = "");

    void eventCheck(sf::Event &event);

    void draw(bool hidden = false);

    std::string getStr();

private:
    bool isLoginOrPassword;
    sf::String input;
    sf::Text text, cursor;
    sf::Font font;
    sf::RectangleShape entry;
    bool isActive;
    int symbolsCount;
    sf::Clock clock;
    const std::string availableLetters = {"abcdefghijklmnopqrstuvwxyz0123456789"};
    const std::string availableSymbols = {",.?!- +=*/@#$&()'\\|~`"};
    std::function<void()> enterPressedFunc;
};

class Title : public ScreenObject {
public:
    Title(const std::string &text_, sf::Vector2<float> position, std::shared_ptr<sf::RenderWindow> window_,
          int size = 24, sf::Color color_ = sf::Color::Black, const std::string &font_ = "Upheavtt.ttf");

    void setText(const std::string &newText);

    void setColor(sf::Color clr);

    sf::FloatRect getSize() const;

    void setPosition(sf::Vector2<float> pos);

    void draw();

private:
    sf::Text text;
    sf::Font font;

};

class Picture : public ScreenObject {
public:
    Picture(const std::string &fileName, sf::Vector2<float> position, sf::Vector2<float> scale_,
            std::shared_ptr<sf::RenderWindow> window_);

    void draw() const;

private:
    sf::Texture texture;
    sf::Sprite sprite;
};

#endif//SCREEN_OBJECTS_H
