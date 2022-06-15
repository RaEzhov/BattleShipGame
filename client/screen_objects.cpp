// Copyright 2022 Roman Ezhov. Github: RaEzhov

#include <thread>
#include <utility>
#include <SFML/System.hpp>

#include "client/screen_objects.h"

ShipDirection& operator++(ShipDirection &other) {
  switch (other) {
    case UP:
      other = RIGHT;
      break;
    case RIGHT:
      other = DOWN;
      break;
    case DOWN:
      other = LEFT;
      break;
    case LEFT:
      other = UP;
      break;
  }
  return other;
}

const ShipDirection operator++(ShipDirection &other, int) {
  ShipDirection rVal = other;
  ++other;
  return rVal;
}

Button::Button(
    sf::Vector2<float> position_, sf::Vector2<float> scale_,
    std::function<void()> funcRef, std::shared_ptr<sf::RenderWindow> window_,
    const std::string &text_, unsigned int textSize, sf::Color textColor_,
    const std::string &font, const std::string &buttonOn,
    const std::string &buttonOff, const std::string &soundFile)
    : ScreenObject(
        std::move(window_)), function(std::move(funcRef)), scale(scale_),
        buttonPosition(position_), lockClick(false), pressed(false),
        textColor(textColor_), sound(soundFile) {
  // Loading sprites
  textFont.loadFromFile(Config::instance().resources + font);
  text.setFont(textFont);
  text.setCharacterSize(textSize);
  text.setFillColor(textColor);
  text.setString(text_);
  textureButtonOn.loadFromFile(Config::instance().resources + buttonOn);
  textureButtonOff.loadFromFile(Config::instance().resources + buttonOff);
  spriteButtonOn.setTexture(textureButtonOn, true);
  spriteButtonOff.setTexture(textureButtonOff, true);


  // Scaling button
  auto newScale = sf::Vector2<float>(Config::instance().buttonScale * scale.x,
                                     Config::instance().buttonScale * scale.y);
  text.setScale(newScale);
  spriteButtonOn.scale(newScale);
  spriteButtonOff.scale(newScale);

  spriteButtonOn.setPosition(buttonPosition);
  spriteButtonOff.setPosition(buttonPosition);
  text.setPosition({
    spriteButtonOn.getPosition().x +
    (static_cast<float>(textureButtonOn.getSize().x)
    - text.getLocalBounds().width) * scale.x,
    spriteButtonOn.getPosition().y +
    (static_cast<float>(textureButtonOn.getSize().y)
    - text.getLocalBounds().height -
    static_cast<float>(text.getCharacterSize())) * scale.y});
  textPosition = text.getPosition();

  spriteButtonOn.setColor(sf::Color(225, 225, 225, 255));
}

void Button::draw() {
  if (!pressed) {
    text.setFillColor(textColor);
    text.setPosition(textPosition);
    window->draw(spriteButtonOn);
  } else {
    text.setFillColor(textColor - sf::Color(10, 10, 10, 10));
    text.setPosition(textPosition.x,
                     textPosition.y
                         + 2 * Config::instance().buttonScale * scale.y);
    window->draw(spriteButtonOff);
  }
  window->draw(text);
}

void Button::eventCheck(const sf::Event &event) {
  auto mouse = static_cast<sf::Vector2<float>>(sf::Mouse::getPosition(*window));
  if (spriteButtonOn.getGlobalBounds().contains(mouse)) {
    spriteButtonOn.setColor(sf::Color::White);
  } else {
    spriteButtonOn.setColor(sf::Color(225, 225, 225, 255));
    pressed = false;
  }
  if (event.type == sf::Event::MouseButtonPressed) {
    if (event.mouseButton.button == sf::Mouse::Left && !lockClick) {
      if (spriteButtonOn.getGlobalBounds().contains(mouse)) {
        pressed = true;
      }
      lockClick = true;
    }
  }
  if (event.type == sf::Event::MouseButtonReleased) {
    if (event.mouseButton.button == sf::Mouse::Left) {
      if (spriteButtonOn.getGlobalBounds().contains(mouse)) {
        pressed = false;
        spriteButtonOn.setColor(sf::Color(225, 225, 225, 255));
        sound.play();
        if (function) {
          function();
          return;
        } else {
          window->close();
        }
      }
      lockClick = false;
    }
  }
}

void Button::move(sf::Vector2<float> offset) {
  text.move(offset);
  spriteButtonOn.move(offset);
  spriteButtonOff.move(offset);
}

void Button::press() {
  if (function) {
    function();
  }
}

sf::Rect<float> Button::getSize() const {
  return spriteButtonOn.getGlobalBounds();
}

Entry::Entry(sf::Vector2<float> position,
             sf::Vector2<float> scale,
             unsigned int size,
             std::shared_ptr<sf::RenderWindow> window_,
             unsigned int fontSize = 24,
             bool isLogOrPass,
             std::function<void()> enterFunc,
             const std::string &str, const std::string &fontFile)
    : ScreenObject(std::move(window_)), isActive(false), symbolsCount(21),
      input(str), isLoginOrPassword(isLogOrPass),
      enterPressedFunc(std::move(enterFunc)) {
  font.loadFromFile(Config::instance().resources + fontFile);
  text.setFont(font);
  text.setCharacterSize(fontSize);
  text.setScale(scale);
  text.setFillColor(sf::Color::Black);
  text.setPosition(position.x, position.y);
  entry.setPosition(position);
  entry.setScale(scale);
  entry.setSize({static_cast<float>(size * fontSize),
                 static_cast<float>(fontSize * 1.3)});
  entry.setFillColor(sf::Color(240, 240, 240, 255));
  text.setString(input);
  clock.restart();
  entry.setOutlineThickness(1);
  entry.setOutlineColor(sf::Color::Black);
  cursor.setString("|");
  cursor.setFont(font);
  cursor.setScale(scale);
  cursor.setCharacterSize(fontSize);
  cursor.setFillColor(sf::Color::Black);
  cursor.setPosition(position);
}

void Entry::eventCheck(const sf::Event &event) {
  if (isActive) {
    if (event.type == sf::Event::TextEntered) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        isActive = false;
        entry.setFillColor(sf::Color(240, 240, 240, 255));
      } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace)) {
        if (!input.isEmpty()) {
          input.erase(input.getSize() - 1);
        }
      } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab)) {
        if (!isLoginOrPassword) {
          input += ' ';
        }
      } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)
          && enterPressedFunc != nullptr) {
        enterPressedFunc();
      } else if (input.getSize() < symbolsCount) {
        if (isLoginOrPassword) {
          if (availableLetters.find(static_cast<char>(event.text.unicode))
              != std::string::npos && input.operator std::string().size() < 8) {
            input += event.text.unicode;
          }
        } else {
          if (availableLetters.find(static_cast<char>(event.text.unicode))
              != std::string::npos ||
              availableSymbols.find(static_cast<char>(event.text.unicode))
                  != std::string::npos) {
            input += event.text.unicode;
          }
        }
      }
      text.setString(input);
      cursor.setPosition(
          text.getPosition().x + static_cast<float>(text.getCharacterSize()),
          text.getPosition().y);
    }
  }

  if (event.type == sf::Event::MouseButtonPressed) {
    if (event.mouseButton.button == sf::Mouse::Left) {
      if (sf::IntRect(static_cast<int>(entry.getPosition().x),
                      static_cast<int>(entry.getPosition().y),
                      static_cast<int>(entry.getSize().x),
                      static_cast<int>(entry.getSize().y)).contains(
          sf::Mouse::getPosition(*window))) {
        isActive = true;
        entry.setFillColor(sf::Color::White);
      } else {
        isActive = false;
        entry.setFillColor(sf::Color(240, 240, 240, 255));
      }
    }
  }
}

void Entry::draw(bool hidden) {
  window->draw(entry);
  auto text_ = text;
  if (hidden) {
    text_.setString(std::string(text_.getString().getSize(), '*'));
  }
  if (isActive && (clock.getElapsedTime().asMilliseconds() % 1000) > 500) {
    cursor = text_;
    cursor.setString(text_.getString() + "|");
    window->draw(cursor);
  } else {
    window->draw(text_);
  }
}

std::string Entry::getStr() const {
  return text.getString();
}

Title::Title(const std::string &text_,
             sf::Vector2<float> position,
             sf::Vector2<float> scale_,
             std::shared_ptr<sf::RenderWindow> window_,
             int size,
             sf::Color color_,
             const std::string &font_) : ScreenObject(std::move(window_)) {
  font.loadFromFile(Config::instance().resources + font_);
  text.setString(text_);
  text.setPosition(position);
  text.setFont(font);
  text.setFillColor(color_);
  text.setCharacterSize(size);
  text.setScale(scale_);
}

void Title::setText(const std::string &newText) {
  text.setString(newText);
}

void Title::setColor(sf::Color clr) {
  text.setFillColor(clr);
  text.setOutlineColor(sf::Color::White);
}

sf::FloatRect Title::getSize() const {
  return text.getGlobalBounds();
}

void Title::setPosition(sf::Vector2<float> pos) {
  text.setPosition(pos);
}

void Title::draw() {
  window->draw(text);
}

std::string Title::getText() const {
  return text.getString();
}

void Title::move(sf::Vector2<float> offset) {
  text.move(offset);
}

TitleRef::TitleRef(
    const std::string &text_, sf::Vector2<float> position,
    sf::Vector2<float> scale_, std::shared_ptr<sf::RenderWindow> window_,
    int size, std::function<void()> func, sf::Color color_,
    const std::string &font_) : Title(
        text_, position, scale_, std::move(window_), size, color_, font_),
    function(std::move(func)) {}

void TitleRef::eventCheck(const sf::Event &event) {
  if (text.getGlobalBounds().contains(
      static_cast<float>(sf::Mouse::getPosition(*window).x),
      static_cast<float>(sf::Mouse::getPosition(*window).y))) {
    text.setStyle(sf::Text::Underlined);
    if (event.type == sf::Event::MouseButtonReleased) {
      if (function) {
        function();
      }
    }
  } else {
    text.setStyle(sf::Text::Regular);
  }
}

Picture::Picture(
    const std::string &fileName, sf::Vector2<float> position,
    sf::Vector2<float> scale_, std::shared_ptr<sf::RenderWindow> window_) :
    ScreenObject(std::move(window_)) {
  texture.loadFromFile(Config::instance().resources + fileName);
  texture.setSmooth(false);
  sprite.setTexture(texture);
  sprite.setScale(scale_);
  sprite.setPosition(position);
}

void Picture::draw() const {
  window->draw(sprite);
}

void Picture::setPosition(sf::Vector2<float> newPosition) {
  sprite.setPosition(newPosition);
}

sf::Rect<float> Picture::getSize() const {
  return sprite.getGlobalBounds();
}

Animation::Animation(
    sf::Vector2<float> position_, sf::Vector2<float> scale_,
    unsigned int frames, float speed_, const std::string &filename,
    std::shared_ptr<sf::RenderWindow> window_) : ScreenObject(
        std::move(window_)), speed(speed_), isPlaying(false) {
  for (unsigned int i = 1; i <= frames; i++) {
    textures.emplace_back();
    (--(textures.end()))->loadFromFile(
        Config::instance().resources + std::to_string(i) + filename);
    sprites.emplace_back(*(--(textures.end())));
    (--(sprites.end()))->setScale(scale_);
    (--(sprites.end()))->setPosition(position_);
  }
  sprite = sprites.begin();
}

void Animation::draw() {
  window->draw(*sprite);
}

sf::Rect<float> Animation::getSize() const {
  return sprite->getGlobalBounds();
}

bool Animation::isAnimationPlaying() const {
  return isPlaying;
}

void Animation::playAnimation(bool reverse) {
  clock.restart();
  if (!reverse) {
    sprite = sprites.begin();
  } else {
    sprite = --(sprites.end());
  }
  std::thread thread([this, reverse] { changeSprites(reverse); });
  thread.detach();
}

void Animation::changeSprites(bool reverse) {
  isPlaying = true;
  while (true) {
    if (clock.getElapsedTime() > sf::seconds(1) / speed) {
      if (!reverse) {
        if (++sprite == sprites.end()) {
          sprite = sprites.begin();
          isPlaying = false;
          return;
        }
      } else {
        if (sprite-- == sprites.begin()) {
          sprite = --(sprites.end());
          isPlaying = false;
          return;
        }
      }
      clock.restart();
    }
  }
}

Pages::Pages(
    sf::Vector2<float> position_, sf::Vector2<float> scale_,
    std::shared_ptr<sf::RenderWindow> window_) : ScreenObject(
        std::move(window_)), position(position_), scale(scale_), lastPosNum{} {
  background = std::make_unique<Animation>(
      position, scale * 8.f, 5, 10.f, "rotating.png", window);

  auto temp = std::make_unique<Button>(
      position, scale, nullptr, window, "", 20, beige,
      "Upheavtt.ttf", "squareButton1.png", "squareButton2.png");
  auto bgSize = background->getSize();
  next = std::make_unique<Button>(
      sf::Vector2<float>{position.x + bgSize.width + 10.f,
                         position.y + bgSize.height * 0.5f
                         - temp->getSize().height / 2.f},
      scale, [this] { nextPage(); }, window, ">", 20, beige,
      "Upheavtt.ttf", "squareButton1.png", "squareButton2.png");
  previous = std::make_unique<Button>(
      sf::Vector2<float>{ position.x - 10.f - temp->getSize().width,
                          position.y + bgSize.height * 0.5f
                          - temp->getSize().height / 2.f},
      scale, [this] { previousPage(); }, window, "<", 20, beige,
      "Upheavtt.ttf", "squareButton1.png", "squareButton2.png");
  temp.reset(nullptr);

  auto tempPos = position;
  tempPos.x += 100.f * scale.x;
  tempPos.y += 50.f * scale.y;
  for (int i = 0; i < 10; i++) {
    if (i == 5) {
      tempPos.x = position.x + 390.f * scale.x;
      tempPos.y = position.y + 50.f * scale.y;
    }
    tempPos.y += 70.f * scale.y;
    positions.push_back(tempPos);
  }
}

void Pages::eventCheck(const sf::Event &event) {
  previous->eventCheck(event);
  next->eventCheck(event);
  auto temp_drown = drown;
  for (int i = 0; !words.empty() && temp_drown != words.end() && i < 10; i++) {
    temp_drown->eventCheck(event);
    temp_drown++;
  }
}

void Pages::draw() {
  background->draw();
  previous->draw();
  next->draw();
  if (background->isAnimationPlaying()) {
    return;
  }
  auto temp_drown = drown;
  for (int i = 0; !words.empty() && temp_drown != words.end() && i < 10; i++) {
    temp_drown->draw();
    temp_drown++;
  }
}

void Pages::addTitle(const std::string &string, std::function<void()> func) {
  words.emplace_back(string, positions[lastPosNum], scale, window, 40, func);
  lastPosNum = (lastPosNum + 1) % 10;
  if (words.size() == 1) {
    drown = words.begin();
  }
}

void Pages::nextPage() {
  auto tempIter = drown;
  int i = 0;
  for (; tempIter != --(words.end()) && i < 10; i++) {
    tempIter++;
  }
  if (i == 10) {
    drown = tempIter;
    background->playAnimation();
  }
}

void Pages::previousPage() {
  auto tempIter = drown;
  int i = 0;
  for (; tempIter != words.begin() && i < 10; i++) {
    tempIter--;
  }
  if (i == 10) {
    drown = tempIter;
    background->playAnimation(true);
  }
}

void Pages::clearTitles() {
  words.clear();
  drown = words.end();
  lastPosNum = 0;
}

Notification::Notification(
    const std::string &text, char id_, sf::Vector2<float> position,
    sf::Vector2<float> scale, std::shared_ptr<sf::RenderWindow> window_,
    std::function<void()> function) :
    ScreenObject(std::move(window_)),
    background("notification.png", position, scale, window), info(
        text, sf::Vector2<float>{background.getSize().left + 10.f * scale.x,
                                 background.getSize().top + 15.f * scale.y},
        scale * 1.3f, window), close(sf::Vector2<float>{
          background.getSize().left + background.getSize().width
          - 40.f * scale.x,
          background.getSize().top + 10.f * scale.y}, scale * 0.5f,
          std::move(function), window, "", 25, sf::Color::White,
          "Upheavtt.ttf", "cross.png", "cross.png"), id(id_), timer() {}

void Notification::eventCheck(const sf::Event &event) {
  close.eventCheck(event);
}

sf::Rect<float> Notification::getSize() const {
  return background.getSize();
}

void Notification::draw() {
  background.draw();
  info.draw();
  close.draw();
}

void Notification::moveUp() {
  auto pos = getSize();
  float offset = pos.height + 10.f;
  background.setPosition({pos.left, pos.top - offset});
  info.move({0, -offset});
  close.move({0, -offset});
}

NotificationPool::NotificationPool(
    sf::Vector2<float> scale_, std::shared_ptr<sf::RenderWindow> window_) :
    ScreenObject(std::move(window_)), scale(scale_), notifications(),
    nextId(0) {}

void NotificationPool::eventCheck(const sf::Event &event) {
  for (auto iter = notifications.begin(); iter != notifications.end();) {
    auto next = iter;
    next++;
    iter->eventCheck(event);
    iter = next;
  }
  for (auto i : moveList) {
    i->moveUp();
  }
  if (!moveList.empty()) {
    sf::Event::MouseWheelScrollEvent();
  }
  moveList.clear();
}

void NotificationPool::draw() {
  for (auto iter = notifications.begin(); iter != notifications.end();) {
    auto next = iter;
    next++;
    iter->draw();
    if (iter->timer.getElapsedTime() >= sf::seconds(5)) {
      deleteNotification(iter->id);
      for (auto i : moveList) {
        i->moveUp();
      }
      moveList.clear();
    }
    iter = next;
  }
}

void NotificationPool::addNotification(const std::string &info) {
  auto id = nextId++;
  if (notifications.empty()) {
    notifications.emplace_back(
        info, id, sf::Vector2{10.f, 10.f}, scale, window,
        [this, id] { deleteNotification(id); });
    return;
  }
  auto position = (--(notifications.end()))->getSize();
  position.top += position.height + 10.f;
  notifications.emplace_back(
      info, id, sf::Vector2{position.left, position.top},
      scale, window, [this, id] { deleteNotification(id); });
}

void NotificationPool::deleteNotification(char id) {
  for (auto n = notifications.begin(); n != notifications.end(); n++) {
    if (n->id == id) {
      auto del = n;
      n++;
      while (n != notifications.end()) {
        moveList.push_back(n);
        n++;
      }
      notifications.erase(del);
      return;
    }
  }
}
