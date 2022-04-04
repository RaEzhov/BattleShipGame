#ifndef SCREENOBJECTS_H
#define SCREENOBJECTS_H

#include <memory>
#include <SFML/Graphics.hpp>
#include <utility>

using namespace sf;

const char RESOURCES_PATH[] = "/home/roman/clionProjects/BattleShipGame/client/resources/";

const float BUTTON_SCALE = 2;

const char IP_ADDR[] = "localhost";

const int PORT = 55555;

const unsigned int FRAMERATE = 30;

class ScreenObject {
public:
    explicit ScreenObject(std::shared_ptr<RenderWindow> &window_) : window(window_) {}

protected:
    std::shared_ptr<RenderWindow> window;
};

class Button : public ScreenObject {
public:
    Button(float x, float y, Vector2<float> scale_, std::function<void()> funcRef, std::shared_ptr<RenderWindow> window_,
           const std::string &textTitle,
           const std::string &buttonOn = std::string(RESOURCES_PATH) + "button1.png",
           const std::string &buttonOff = std::string(RESOURCES_PATH) + "button2.png");

    void draw();

    void eventCheck(Event &event);

private:
    Texture textureButtonOn, textureButtonOff, textureTitle;
    Sprite spriteButtonOn, spriteButtonOff, spriteTitle;
    bool lockClick;
    bool pressed;
    Vector2<float> buttonPosition, titlePosition;
    Vector2<float> scale;

    std::function<void()> function;
};

class Entry : public ScreenObject {
public:
    Entry(Vector2<float> position, unsigned int size, std::shared_ptr<RenderWindow> window_, unsigned int fontSize = 24);

    void eventCheck(Event &event);

    void draw();

    std::string getStr() {
        return text.getString();
    }

private:
    String input;
    Text text, cursor;
    Font font;
    RectangleShape entry;
    bool isActive;
    int symbolsCount;
    Clock clock;
};

#endif//SCREENOBJECTS_H
