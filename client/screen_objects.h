// Copyright 2022 Roman Ezhov. Github: RaEzhov

#ifndef CLIENT_SCREEN_OBJECTS_H_
#define CLIENT_SCREEN_OBJECTS_H_

#include <utility>
#include <memory>
#include <vector>
#include <string>
#include <list>
#include <functional>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#include "client/config.h"
#include "global/message_status.h"

const auto beige = sf::Color(225, 225, 225, 255);

enum GameFieldState {
  PLACEMENT,
  INACTIVE,
  GAME,
  ENEMY_INACTIVE
};

enum ShipState {
  ALIVE,
  INJURED,
  DESTROYED
};

enum ShipDirection {
  UP = 0,
  RIGHT = 1,
  DOWN = 2,
  LEFT = 3
};

enum ShipCheckStatus {
  REMOVED,
  ROTATED,
  NONE
};

ShipDirection& operator++(ShipDirection &other);

const ShipDirection operator++(ShipDirection &other, int);

class ScreenObject {
 public:
  explicit ScreenObject(std::shared_ptr<sf::RenderWindow> window_) :
  window(std::move(window_)) {}

 protected:
  std::shared_ptr<sf::RenderWindow> window;
};

class Sound {
 public:
  explicit Sound(const std::string &file);

  void play();

 private:
  sf::SoundBuffer buffer;
  sf::Sound sound;
};

class Button : public ScreenObject {
 public:
  Button(sf::Vector2<float> position_,
         sf::Vector2<float> scale_,
         std::function<void()> funcRef,
         std::shared_ptr<sf::RenderWindow> window_,
         const std::string &text_,
         unsigned int textSize,
         sf::Color textColor_,
         const std::string &font = "Upheavtt.ttf",
         const std::string &buttonOn = "button1.png",
         const std::string &buttonOff = "button2.png",
         const std::string &soundFile = "button.wav");

  void draw();

  void press();

  void eventCheck(const sf::Event &event);

  void move(sf::Vector2<float> offset);

  sf::Rect<float> getSize() const;

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
  Sound sound;

  std::function<void()> function;
};

class Entry : public ScreenObject {
 public:
  Entry(sf::Vector2<float> position,
        sf::Vector2<float> scale,
        unsigned int size,
        std::shared_ptr<sf::RenderWindow> window_,
        unsigned int fontSize,
        bool isLogOrPass = true,
        std::function<void()> enterFunc = nullptr,
        const std::string &str = "",
        const std::string &fontFile = "Upheavtt.ttf");

  void eventCheck(const sf::Event &event);

  void draw(bool hidden = false);

  std::string getStr() const;

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
  Title(const std::string &text_,
        sf::Vector2<float> position,
        sf::Vector2<float> scale_,
        std::shared_ptr<sf::RenderWindow> window_,
        int size = 24,
        sf::Color color_ = sf::Color::Black,
        const std::string &font_ = "Upheavtt.ttf");

  void setText(const std::string &newText);

  std::string getText() const;

  void move(sf::Vector2<float> offset);

  void setColor(sf::Color clr);

  sf::FloatRect getSize() const;

  void setPosition(sf::Vector2<float> pos);

  void draw();

 private:
  sf::Font font;

 protected:
  sf::Text text;
};

class TitleRef : public Title {
 public:
  TitleRef(const std::string &text_,
           sf::Vector2<float> position,
           sf::Vector2<float> scale_,
           std::shared_ptr<sf::RenderWindow> window_,
           int size = 24,
           std::function<void()> func = nullptr,
           sf::Color color_ = sf::Color::Black,
           const std::string &font_ = "Upheavtt.ttf");

  void eventCheck(const sf::Event &event);

 private:
  std::function<void()> function;
};

class Picture : public ScreenObject {
 public:
  Picture(const std::string &fileName,
          sf::Vector2<float> position,
          sf::Vector2<float> scale_,
          std::shared_ptr<sf::RenderWindow> window_);

  void draw() const;

  void setPosition(sf::Vector2<float> newPosition);

  sf::Rect<float> getSize() const;

 private:
  sf::Texture texture;
  sf::Sprite sprite;
};

class Animation : public ScreenObject {
 public:
  Animation(sf::Vector2<float> position_,
            sf::Vector2<float> scale_,
            unsigned int frames,
            float speed_,
            const std::string &filename,
            std::shared_ptr<sf::RenderWindow> window_);

  void draw();

  sf::Rect<float> getSize() const;

  bool isAnimationPlaying() const;

  void playAnimation(bool reverse = false);

 private:
  void changeSprites(bool reverse);

  std::list<sf::Texture> textures;
  std::list<sf::Sprite> sprites;
  std::list<sf::Sprite>::iterator sprite;
  float speed;
  sf::Clock clock;
  bool isPlaying;
};

class Pages : public ScreenObject {
 public:
  Pages(sf::Vector2<float> position,
        sf::Vector2<float> scale_,
        std::shared_ptr<sf::RenderWindow> window_);

  void eventCheck(const sf::Event &event);

  void draw();

  void addTitle(const std::string &string,
                std::function<void()> func = nullptr);

  void clearTitles();

 private:
  void nextPage();

  void previousPage();

  std::unique_ptr<Button> previous, next;
  std::unique_ptr<Animation> background;
  std::list<TitleRef> words;
  size_t lastPosNum;
  std::list<TitleRef>::iterator drown;
  std::vector<sf::Vector2<float>> positions;
  sf::Vector2<float> position, scale;
};

class Notification : public ScreenObject {
 public:
  friend class NotificationPool;

  Notification(const std::string &text,
               char id_,
               sf::Vector2<float> position,
               sf::Vector2<float> scale,
               std::shared_ptr<sf::RenderWindow> window_,
               std::function<void()> function);

  void eventCheck(const sf::Event &event);

  sf::Rect<float> getSize() const;

  void draw();

  void moveUp();

 private:
  Picture background;
  Title info;
  Button close;
  char id;
  sf::Clock timer;
};

class NotificationPool : public ScreenObject {
 public:
  explicit NotificationPool(sf::Vector2<float> scale_,
                            std::shared_ptr<sf::RenderWindow> window_);

  void eventCheck(const sf::Event &event);

  void draw();

  void addNotification(const std::string &info);

 private:
  void deleteNotification(char id);

  std::list<Notification> notifications;
  std::list<std::list<Notification>::iterator> moveList;
  sf::Vector2<float> scale;
  char nextId;
};

#endif  // CLIENT_SCREEN_OBJECTS_H_
