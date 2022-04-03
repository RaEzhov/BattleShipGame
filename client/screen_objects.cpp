#include "screen_objects.h"

Button::Button(float x, float y, Vector2<float> scale_, std::function<void()> funcRef, std::shared_ptr<RenderWindow> window_,
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
        spriteTitle.setColor(Color::White);
        spriteTitle.setPosition(titlePosition);
        window->draw(spriteButtonOn);
    } else {
        spriteTitle.setColor(Color(200, 200, 200));
        spriteTitle.setPosition(titlePosition.x, titlePosition.y + 2 * BUTTON_SCALE * scale.y);
        window->draw(spriteButtonOff);
    }
    window->draw(spriteTitle);
}

void Button::eventCheck(Event &event){
    if (IntRect(static_cast<int>(spriteButtonOn.getPosition().x), static_cast<int>(spriteButtonOn.getPosition().y),
                textureButtonOn.getSize().x * BUTTON_SCALE * scale.x,
                textureButtonOn.getSize().y * BUTTON_SCALE * scale.y).contains(
            Mouse::getPosition(*window))) {
        spriteButtonOn.setColor(Color::White);
    } else {
        spriteButtonOn.setColor(Color(225, 225, 225, 255));
        pressed = false;
    }
    if (event.type == Event::MouseButtonPressed) {
        if (event.mouseButton.button == Mouse::Left && !lockClick) {
            if (IntRect(static_cast<int>(spriteButtonOn.getPosition().x), static_cast<int>(spriteButtonOn.getPosition().y),
                        textureButtonOn.getSize().x * BUTTON_SCALE * scale.x,
                        textureButtonOff.getSize().y * BUTTON_SCALE * scale.y).contains(
                    Mouse::getPosition(*window))) {
                pressed = true;
            }
            lockClick = true;
        }
    }
    if (event.type == Event::MouseButtonReleased) {
        if (event.mouseButton.button == Mouse::Left) {
            if (IntRect(static_cast<int>(spriteButtonOn.getPosition().x), static_cast<int>(spriteButtonOn.getPosition().y),
                        textureButtonOn.getSize().x * BUTTON_SCALE * scale.x,
                        textureButtonOff.getSize().y * BUTTON_SCALE * scale.y).contains(
                    Mouse::getPosition(*window))) {
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

Entry::Entry(Vector2<float> position, unsigned int size, std::shared_ptr<RenderWindow> window_, unsigned int fontSize):
        ScreenObject(window_), isActive(false), symbolsCount(21), input(""){
    font.loadFromFile(std::string(RESOURCES_PATH) + "arialmt.ttf");
    text.setFont(font);
    text.setCharacterSize(fontSize);
    text.setFillColor(sf::Color::Black);
    text.setPosition(position.x, position.y);
    entry.setPosition(position);
    entry.setSize({static_cast<float>(size * fontSize), static_cast<float>(fontSize * 1.3)});
    entry.setFillColor(Color(230, 218, 166, 195));
    text.setString(input);
    clock.restart();
    entry.setOutlineThickness(1);
    entry.setOutlineColor(Color::Black);
    cursor.setString("|");
    cursor.setFont(font);
    cursor.setCharacterSize(fontSize);
    cursor.setFillColor(Color::Black);
    cursor.setPosition(position);
}

void Entry::eventCheck(Event &event)  {
    if (isActive) {
        if (event.type == sf::Event::TextEntered) {
            if (Keyboard::isKeyPressed(Keyboard::Escape)) {
                isActive = false;
                entry.setFillColor(Color(230, 218, 166, 195));
            } else if (Keyboard::isKeyPressed(Keyboard::BackSpace)) {
                if (!input.isEmpty()) {
                    input.erase(input.getSize() - 1);
                }
            } else if (Keyboard::isKeyPressed(Keyboard::Tab)) {
                input += ' ';
            } else if (Keyboard::isKeyPressed(Keyboard::Enter) || Keyboard::isKeyPressed(Keyboard::Delete)) {

            } else if (input.getSize() < symbolsCount) {
                input += event.text.unicode;
            }
            text.setString(input);
            cursor.setPosition(text.getPosition().x + static_cast<float>(text.getCharacterSize()), text.getPosition().y);
        }
    }

    if (event.type == Event::MouseButtonPressed) {

        if (event.mouseButton.button == Mouse::Left) {
            if (IntRect(static_cast<int>(entry.getPosition().x), static_cast<int>(entry.getPosition().y),
                        static_cast<int>(entry.getSize().x), static_cast<int>(entry.getSize().y)).contains(
                    Mouse::getPosition(*window))) {
                isActive = true;
                entry.setFillColor(Color(230, 218, 166, 255));
            } else {
                isActive = false;
                entry.setFillColor(Color(230, 218, 166, 195));
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