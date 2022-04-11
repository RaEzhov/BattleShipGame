#ifndef SCREENOBJECTS_H
#define SCREENOBJECTS_H

#include <memory>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

const char RESOURCES_PATH[] = "/home/roman/clionProjects/BattleShipGame/client/resources/";

const float BUTTON_SCALE = 2;

const char IP_ADDR[] = "localhost";

const int PORT = 55555;

const unsigned int FRAMERATE = 30;

class ScreenObject {
public:
    explicit ScreenObject(std::shared_ptr<sf::RenderWindow>& window_) : window(window_) {}

protected:
    std::shared_ptr<sf::RenderWindow> window;
};

class Button : public ScreenObject {
public:
    Button(float x, float y, sf::Vector2<float> scale_, std::function<void()> funcRef,
           std::shared_ptr<sf::RenderWindow> window_, const std::string &text_, unsigned int textSize, sf::Color textColor_,
           const std::string &font = "Upheavtt.ttf", const std::string &buttonOn = std::string(RESOURCES_PATH) + "button1.png",
           const std::string &buttonOff = std::string(RESOURCES_PATH) + "button2.png");

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
    Entry(sf::Vector2<float> position, unsigned int size, std::shared_ptr<sf::RenderWindow> window_, unsigned int fontSize);

    void eventCheck(sf::Event &event);

    void draw();

    std::string getStr() {
        return text.getString();
    }

private:
    sf::String input;
    sf::Text text, cursor;
    sf::Font font;
    sf::RectangleShape entry;
    bool isActive;
    int symbolsCount;
    sf::Clock clock;
};

class Title : public ScreenObject {
public:
    Title(const std::string &text_, sf::Vector2<float> position, std::shared_ptr<sf::RenderWindow> window_,
          const std::string &font_ = "arialmt.ttf", int size = 24, sf::Color color_ = sf::Color::Black);

    void setText(const std::string &newText) {
        text.setString(newText);
    }

    void draw() {
        window->draw(text);
    }

private:
    sf::Text text;
    sf::Font font;

};

class Picture : public ScreenObject {
public:
    Picture(const std::string &fileName, sf::Vector2<float> position, sf::Vector2<float> scale_,
            std::shared_ptr<sf::RenderWindow> window_) : ScreenObject(window_) {
        texture.loadFromFile(std::string(RESOURCES_PATH) + fileName);
        texture.setSmooth(false);
        sprite.setTexture(texture);
        sprite.setScale(scale_);
        sprite.setPosition(position);
    }

    void draw() const {
        window->draw(sprite);
    }

private:
    sf::Texture texture;
    sf::Sprite sprite;
};

#endif//SCREENOBJECTS_H
