#include "screen_objects.h"

Button::Button(float x, float y, sf::Vector2<float> scale_, std::function<void()> funcRef, std::shared_ptr<sf::RenderWindow> window_,
               const std::string &textTitle, const std::string &buttonOn, const std::string &buttonOff) :
        ScreenObject(window_), function(std::move(funcRef)), scale(scale_), buttonPosition({x, y}), lockClick(false), pressed(false){

    // Loading sprites
    textureTitle.loadFromFile(textTitle);
    textureButtonOn.loadFromFile(buttonOn);
    textureButtonOff.loadFromFile(buttonOff);
    spriteTitle.setTexture(textureTitle, true);
    spriteButtonOn.setTexture(textureButtonOn, true);
    spriteButtonOff.setTexture(textureButtonOff, true);


    // Scaling button
    spriteTitle.scale(BUTTON_SCALE * scale.x, BUTTON_SCALE * scale.y);
    spriteButtonOn.scale(BUTTON_SCALE * scale.x, BUTTON_SCALE * scale.y);
    spriteButtonOff.scale(BUTTON_SCALE * scale.x, BUTTON_SCALE * scale.y);


    spriteButtonOn.setPosition(buttonPosition);
    spriteButtonOff.setPosition(buttonPosition);
    spriteTitle.setPosition(buttonPosition.x + (float(textureButtonOn.getSize().x) * spriteButtonOn.getScale().x -
                                                float(textureTitle.getSize().x) * spriteTitle.getScale().x) / 2,
                            buttonPosition.y + (float(textureButtonOn.getSize().y) * spriteButtonOn.getScale().y -
                                                float(textureTitle.getSize().y) * spriteTitle.getScale().y) / 2);
    titlePosition = spriteTitle.getPosition();
}

void Button::draw() {
    if (!pressed) {
        spriteTitle.setColor(sf::Color::White);
        spriteTitle.setPosition(titlePosition);
        window->draw(spriteButtonOn);
    } else {
        spriteTitle.setColor(sf::Color(200, 200, 200));
        spriteTitle.setPosition(titlePosition.x, titlePosition.y + 2 * BUTTON_SCALE * scale.y);
        window->draw(spriteButtonOff);
    }
    window->draw(spriteTitle);
}

void Button::eventCheck(sf::Event &event){
    if (sf::IntRect(static_cast<int>(spriteButtonOn.getPosition().x), static_cast<int>(spriteButtonOn.getPosition().y),
                textureButtonOn.getSize().x * BUTTON_SCALE * scale.x,
                textureButtonOn.getSize().y * BUTTON_SCALE * scale.y).contains(
            sf::Mouse::getPosition(*window))) {
        spriteButtonOn.setColor(sf::Color::White);
    } else {
        spriteButtonOn.setColor(sf::Color(225, 225, 225, 255));
        pressed = false;
    }
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left && !lockClick) {
            if (sf::IntRect(static_cast<int>(spriteButtonOn.getPosition().x), static_cast<int>(spriteButtonOn.getPosition().y),
                        textureButtonOn.getSize().x * BUTTON_SCALE * scale.x,
                        textureButtonOff.getSize().y * BUTTON_SCALE * scale.y).contains(
                    sf::Mouse::getPosition(*window))) {
                pressed = true;
            }
            lockClick = true;
        }
    }
    if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            if (sf::IntRect(static_cast<int>(spriteButtonOn.getPosition().x), static_cast<int>(spriteButtonOn.getPosition().y),
                        textureButtonOn.getSize().x * BUTTON_SCALE * scale.x,
                        textureButtonOff.getSize().y * BUTTON_SCALE * scale.y).contains(
                    sf::Mouse::getPosition(*window))) {
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

Entry::Entry(sf::Vector2<float> position, unsigned int size, std::shared_ptr<sf::RenderWindow> window_, unsigned int fontSize = 24):
        ScreenObject(window_), isActive(false), symbolsCount(21), input(""){
    font.loadFromFile(std::string(RESOURCES_PATH) + "arialmt.ttf");
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

void Entry::eventCheck(sf::Event &event)  {
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
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) || sf::Keyboard::isKeyPressed(sf::Keyboard::Delete)) {

            } else if (input.getSize() < symbolsCount) {
                input += event.text.unicode;
            }
            text.setString(input);
            cursor.setPosition(text.getPosition().x + static_cast<float>(text.getCharacterSize()), text.getPosition().y);
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

void Entry::draw() {
    window->draw(entry);
    if (isActive && (clock.getElapsedTime().asMilliseconds() % 1000) > 500) {
        cursor = text;
        cursor.setString(text.getString() + "|");
        window->draw(cursor);
    } else {
        window->draw(text);
    }
}

Title::Title(const std::string& text_, sf::Vector2<float> position, std::shared_ptr<sf::RenderWindow> window_, const std::string& font_,int size,
             sf::Color color_): ScreenObject(window_) {
    font.loadFromFile(std::string(RESOURCES_PATH) + font_);
    text.setString(text_);
    text.setPosition(position);
    text.setFont(font);
    text.setFillColor(color_);
    text.setCharacterSize(size);
}
