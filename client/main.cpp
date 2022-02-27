#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

using namespace sf;

const unsigned int FRAMERATE = 60;

const float BUTTON_SCALE = 2;

class Button {
public:
    Button(float x, float y, Vector2<float> scale_, void(*funcRef)(), RenderWindow *window_,
           const std::string &textTitle,
           const std::string &buttonOn = "./resources/button1.png",
           const std::string &buttonOff = "./resources/button2.png") {
        pFunction = funcRef;
        scale = scale_;

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

        buttonPosition = {x, y};

        spriteButtonOn.setPosition(buttonPosition);
        spriteButtonOff.setPosition(buttonPosition);
        spriteTitle.setPosition(buttonPosition.x + (textureButtonOn.getSize().x * spriteButtonOn.getScale().x -
                                                    textureTitle.getSize().x * spriteTitle.getScale().x) / 2,
                                buttonPosition.y + (textureButtonOn.getSize().y * spriteButtonOn.getScale().y -
                                                    textureTitle.getSize().y * spriteTitle.getScale().y) / 2);
        titlePosition = spriteTitle.getPosition();
        drawingSprite = &spriteButtonOn;
        lockClick = false;
        window = window_;
        pressed = 0;
    }

    void draw() {
        if (pressed == 0) {
            spriteTitle.setColor(Color::White);
            spriteTitle.setPosition(titlePosition);
        } else {
            spriteTitle.setColor(Color(200, 200, 200));
            spriteTitle.setPosition(titlePosition.x, titlePosition.y + 2 * BUTTON_SCALE * scale.y);

        }
        window->draw(*drawingSprite);
        window->draw(spriteTitle);
    }

    void eventCheck(Event &event) {
        if (IntRect(spriteButtonOn.getPosition().x, spriteButtonOn.getPosition().y,
                    textureButtonOn.getSize().x * BUTTON_SCALE * scale.x,
                    textureButtonOn.getSize().y * BUTTON_SCALE * scale.y).contains(
                Mouse::getPosition(*window))) {
            spriteButtonOn.setColor(Color::White);
        } else {
            spriteButtonOn.setColor(Color(225, 225, 225, 255));
            pressed = 0;
            drawingSprite = &spriteButtonOn;
        }
        if (event.type == Event::MouseButtonPressed) {
            if (event.mouseButton.button == Mouse::Left && lockClick != true) {
                if (IntRect(spriteButtonOn.getPosition().x, spriteButtonOn.getPosition().y,
                            textureButtonOn.getSize().x * BUTTON_SCALE * scale.x,
                            textureButtonOff.getSize().y * BUTTON_SCALE * scale.y).contains(
                        Mouse::getPosition(*window))) {
                    pressed = 1;
                    drawingSprite = &spriteButtonOff;
                }
                lockClick = true;
            }
        }
        if (event.type == Event::MouseButtonReleased) {
            if (event.mouseButton.button == Mouse::Left) {
                if (IntRect(spriteButtonOn.getPosition().x, spriteButtonOn.getPosition().y,
                            textureButtonOn.getSize().x * BUTTON_SCALE * scale.x,
                            textureButtonOff.getSize().y * BUTTON_SCALE * scale.y).contains(
                        Mouse::getPosition(*window))) {
                    pressed = 0;
                    drawingSprite = &spriteButtonOn;
                    if (pFunction) {
                        (*pFunction)();
                    } else {
                        window->close();
                    }
                }
                lockClick = false;
            }
        }
    }

private:
    Texture textureButtonOn, textureButtonOff, textureTitle;
    Sprite spriteButtonOn, spriteButtonOff, spriteTitle;
    Sprite *drawingSprite;
    bool lockClick;
    RenderWindow *window;
    int pressed;
    Vector2<float> buttonPosition, titlePosition;
    Vector2<float> scale;

    void (*pFunction)();
};


int main() {

    auto screenRes = sf::VideoMode::getDesktopMode();
    // Объект, который, собственно, является главным окном приложения
    RenderWindow window(screenRes, "Battleship", Style::Fullscreen);
    window.setFramerateLimit(FRAMERATE);
    window.setMouseCursorVisible(false);

    // Загрузка текстур из памяти

    Texture background, battleshipText, title, cursor;
    background.loadFromFile("./resources/mainMenu.jpg");
    battleshipText.loadFromFile("./resources/battleship.png");
    cursor.loadFromFile("./resources/cursor.png");


    // Получение разрешения фона (1920х1080)
    unsigned int width = background.getSize().x,
            height = background.getSize().y;

    Vector2<float> scaleWindow = {float(screenRes.width) / float(width), float(screenRes.height) / float(height)};

    background.setSmooth(false);

    // Объявление переменных спрайтов
    Sprite spriteBackground(background), spriteBattleship(battleshipText), spriteCursor(cursor);
    spriteCursor.setScale(2, 2);

    spriteBackground.scale(scaleWindow);

    spriteBattleship.setPosition(float(screenRes.width) * 0.35, float(screenRes.height) * 0.85);
    spriteBattleship.scale(scaleWindow);

    Button exitButton(float(screenRes.width) * 0.05, float(screenRes.height) * 0.85, scaleWindow, nullptr, &window,
                      "./resources/exit.png");
    Event event;
    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            // Пользователь нажал на «крестик» и хочет закрыть окно?
            if (event.type == Event::Closed)
                window.close();
            exitButton.eventCheck(event);
        }
        window.draw(spriteBackground);
        window.draw(spriteBattleship);
        exitButton.draw();
        spriteCursor.setPosition(Mouse::getPosition().x, Mouse::getPosition().y);
        window.draw(spriteCursor);
        window.display();
    }

    return 0;
}