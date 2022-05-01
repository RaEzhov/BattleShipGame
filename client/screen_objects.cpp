#include "screen_objects.h"

Button::Button(float x, float y, sf::Vector2<float> scale_, std::function<void()> funcRef, std::shared_ptr<sf::RenderWindow> window_,
               const std::string &text_, unsigned int textSize, sf::Color textColor_, const std::string &font, const std::string &buttonOn,
               const std::string &buttonOff) : ScreenObject(window_), function(std::move(funcRef)), scale(scale_), buttonPosition({x, y}),
               lockClick(false), pressed(false), textColor(textColor_) {

    // Loading sprites
    textFont.loadFromFile(Config::instance().resources + font);
    text.setFont(textFont);
    text.setCharacterSize(textSize);
    text.setFillColor(textColor);
    text.setString(text_);
    textureButtonOn.loadFromFile(buttonOn);
    textureButtonOff.loadFromFile(buttonOff);
    spriteButtonOn.setTexture(textureButtonOn, true);
    spriteButtonOff.setTexture(textureButtonOff, true);


    // Scaling button
    auto newScale = sf::Vector2<float>(Config::instance().buttonScale * scale.x, Config::instance().buttonScale * scale.y);
    text.setScale(newScale);
    spriteButtonOn.scale(newScale);
    spriteButtonOff.scale(newScale);

    spriteButtonOn.setPosition(buttonPosition);
    spriteButtonOff.setPosition(buttonPosition);
    text.setPosition({spriteButtonOn.getPosition().x +
                      (static_cast<float>(textureButtonOn.getSize().x) - text.getLocalBounds().width) * scale.x,
                      spriteButtonOn.getPosition().y +
                      (static_cast<float>(textureButtonOn.getSize().y) - text.getLocalBounds().height -
                       static_cast<float>(text.getCharacterSize())) * scale.y});
    textPosition = text.getPosition();
}

void Button::draw() {
    if (!pressed) {
        text.setFillColor(textColor);
        text.setPosition(textPosition);
        window->draw(spriteButtonOn);
    } else {
        text.setFillColor(textColor - sf::Color(10, 10, 10, 10));
        text.setPosition(textPosition.x, textPosition.y + 2 * Config::instance().buttonScale * scale.y);
        window->draw(spriteButtonOff);
    }
    window->draw(text);
}

void Button::eventCheck(sf::Event &event) {
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
                if (function) {
                    function();
                } else {
                    window->close();
                }
            }
            lockClick = false;
        }
    }
}

Entry::Entry(sf::Vector2<float> position, unsigned int size, std::shared_ptr<sf::RenderWindow> window_, unsigned int fontSize = 24,
             bool isLogOrPass, std::function<void()> enterFunc, const std::string& str): ScreenObject(window_), isActive(false), symbolsCount(21), input(str),
             isLoginOrPassword(isLogOrPass), enterPressedFunc(std::move(enterFunc)) {
    font.loadFromFile(Config::instance().resources + "Upheavtt.ttf");
    text.setFont(font);
    text.setCharacterSize(fontSize);
    text.setFillColor(sf::Color::Black);
    text.setPosition(position.x, position.y);
    entry.setPosition(position);
    entry.setSize({static_cast<float>(size * fontSize), static_cast<float>(fontSize * 1.3)});
    entry.setFillColor(sf::Color(230, 218, 166, 195));
    text.setString(input);
    clock.restart();
    entry.setOutlineThickness(1);
    entry.setOutlineColor(sf::Color::Black);
    cursor.setString("|");
    cursor.setFont(font);
    cursor.setCharacterSize(fontSize);
    cursor.setFillColor(sf::Color::Black);
    cursor.setPosition(position);
}

void Entry::eventCheck(sf::Event &event) {
    if (isActive) {
        if (event.type == sf::Event::TextEntered) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                isActive = false;
                entry.setFillColor(sf::Color(230, 218, 166, 195));
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace)) {
                if (!input.isEmpty()) {
                    input.erase(input.getSize() - 1);
                }
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab)) {
                input += ' ';
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && enterPressedFunc != nullptr) {
                enterPressedFunc();
            } else if (input.getSize() < symbolsCount) {
                if (isLoginOrPassword) {
                    if (availableLetters.find(static_cast<char>(event.text.unicode)) != std::string::npos) {
                        input += event.text.unicode;
                    }
                } else {
                    if (availableLetters.find(static_cast<char>(event.text.unicode)) != std::string::npos ||
                        availableSymbols.find(static_cast<char>(event.text.unicode)) != std::string::npos) {
                        input += event.text.unicode;
                    }
                }
            }
            text.setString(input);
            cursor.setPosition(text.getPosition().x + static_cast<float>(text.getCharacterSize()),
                               text.getPosition().y);
        }
    }

    if (event.type == sf::Event::MouseButtonPressed) {

        if (event.mouseButton.button == sf::Mouse::Left) {
            if (sf::IntRect(static_cast<int>(entry.getPosition().x), static_cast<int>(entry.getPosition().y),
                            static_cast<int>(entry.getSize().x), static_cast<int>(entry.getSize().y)).contains(
                    sf::Mouse::getPosition(*window))) {
                isActive = true;
                entry.setFillColor(sf::Color(230, 218, 166, 255));
            } else {
                isActive = false;
                entry.setFillColor(sf::Color(230, 218, 166, 195));
            }
        }
    }
}

void Entry::draw(bool hidden) {
    window->draw(entry);
    auto text_ = text;
    if (hidden){
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

std::string Entry::getStr() {
    return text.getString();
}

Title::Title(const std::string& text_, sf::Vector2<float> position, std::shared_ptr<sf::RenderWindow> window_,int size,
             sf::Color color_, const std::string& font_): ScreenObject(window_) {
    font.loadFromFile(Config::instance().resources + font_);
    text.setString(text_);
    text.setPosition(position);
    text.setFont(font);
    text.setFillColor(color_);
    text.setCharacterSize(size);
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

Picture::Picture(const std::string &fileName, sf::Vector2<float> position, sf::Vector2<float> scale_,
                 std::shared_ptr<sf::RenderWindow> window_): ScreenObject(window_) {
    texture.loadFromFile(Config::instance().resources + fileName);
    texture.setSmooth(false);
    sprite.setTexture(texture);
    sprite.setScale(scale_);
    sprite.setPosition(position);
}

void Picture::draw() const {
    window->draw(sprite);
}
