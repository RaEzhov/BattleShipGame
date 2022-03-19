#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include <unordered_set>

using namespace sf;

const char IP_ADDR[] = "localhost";

const int PORT = 55555;

const unsigned int FRAMERATE = 60;

const float BUTTON_SCALE = 2;

const char RESOURCES_PATH[] = "/home/roman/clionProjects/BattleShipGame/client/resources/";

class Button {
public:
    Button(float x, float y, Vector2<float> scale_, void(*funcRef)(), RenderWindow *window_,
           const std::string &textTitle,
           const std::string &buttonOn = std::string(RESOURCES_PATH) + "button1.png",
           const std::string &buttonOff = std::string(RESOURCES_PATH) + "button2.png") {
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
        spriteTitle.setPosition(buttonPosition.x + (float(textureButtonOn.getSize().x) * spriteButtonOn.getScale().x -
                                                    float(textureTitle.getSize().x) * spriteTitle.getScale().x) / 2,
                                buttonPosition.y + (float(textureButtonOn.getSize().y) * spriteButtonOn.getScale().y -
                                                    float(textureTitle.getSize().y) * spriteTitle.getScale().y) / 2);
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
            if (event.mouseButton.button == Mouse::Left && !lockClick) {
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

class Entry {
public:
    Entry(Vector2<float> position, unsigned int size, RenderWindow *window_, unsigned int fontSize = 24) {
        window = window_;
        font.loadFromFile(std::string(RESOURCES_PATH) + "arialmt.ttf");
        text.setFont(font);
        text.setCharacterSize(fontSize);
        text.setFillColor(sf::Color::Black);
        text.setPosition(position.x, position.y);
        entry.setPosition(position);
        entry.setSize({static_cast<float>(size * fontSize), static_cast<float>(fontSize * 1.3)});
        isActive = false;
        symbolsCount = 21;
        entry.setFillColor(Color(230, 218, 166, 195));
        input = "";
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

    void eventCheck(Event &event) {
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
                    1;
                    //playerInput.insert(playerInput.getSize(), "\n");
                } else if (input.getSize() < symbolsCount) {
                    input += event.text.unicode;
                }
                text.setString(input);
                cursor.setPosition(text.getPosition().x + text.getCharacterSize(), text.getPosition().y);
            }
        }
        //-----------------------------
        if (event.type == Event::MouseButtonPressed) {

            if (event.mouseButton.button == Mouse::Left) {
                if (IntRect(entry.getPosition().x, entry.getPosition().y,
                            entry.getSize().x, entry.getSize().y).contains(
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

    void draw() {
        window->draw(entry);
        if ( isActive && (clock.getElapsedTime().asMilliseconds() % 1000) > 500){
            cursor = text;
            cursor.setString(text.getString() + "|");
            window->draw(cursor);
        } else {
            window->draw(text);
        }
    }

    std::string getStr(){
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
    RenderWindow *window;
};

enum status{
    LOGIN = 1,
    MAIN_MENU = 2,
    IN_GAME = 3,
    IN_FRIENDS_MENU = 4
};

static unsigned int userStatus = LOGIN;

std::unordered_map<std::string, Button*> buttons;
std::unordered_map<std::string, Entry*> entries;

static sf::TcpSocket server;

void loginFnc(){
    auto login = entries["login"]->getStr();
    auto password = entries["password"]->getStr();
    Packet packet;
    packet << login << password;
    server.send(packet);
    packet.clear();
    bool authDone;
    server.receive(packet);
    packet >> authDone;
    if (authDone){
        userStatus = MAIN_MENU;
    }
};

int main() {

    sf::Socket::Status status = server.connect(IP_ADDR, PORT);
    if (status != sf::Socket::Done)
    {
        std::cout << "Connection error";
        return 1;
    }

    auto screenRes = sf::VideoMode::getDesktopMode();
    RenderWindow window(screenRes, "Battleship", Style::Fullscreen);
    window.setFramerateLimit(FRAMERATE);
    window.setMouseCursorVisible(false);

    Texture background, battleshipText, title, cursor;
    background.loadFromFile(std::string(RESOURCES_PATH) + "mainMenu.jpg");
    battleshipText.loadFromFile(std::string(RESOURCES_PATH) + "battleship.png");
    cursor.loadFromFile(std::string(RESOURCES_PATH) + "cursor.png");


    // Получение разрешения фона (1920х1080)
    unsigned int width = background.getSize().x,
            height = background.getSize().y;

    Vector2<float> scaleWindow = {float(screenRes.width) / float(width), float(screenRes.height) / float(height)};

    background.setSmooth(false);

    // Объявление переменных спрайтов
    Sprite spriteBackground(background), spriteBattleship(battleshipText), spriteCursor(cursor);
    spriteCursor.setScale(2, 2);

    spriteBackground.scale(scaleWindow);

    spriteBattleship.setPosition(float(screenRes.width) * 0.35f, float(screenRes.height) * 0.85f);
    spriteBattleship.scale(scaleWindow);

    Button exitButton(float(screenRes.width) * 0.05f, float(screenRes.height) * 0.85f, scaleWindow, nullptr, &window,
                      std::string(RESOURCES_PATH) + "exit.png");
    Button loginButton(float(screenRes.width) * 0.22f, float(screenRes.height) * 0.6f, scaleWindow * 0.5f, loginFnc, &window,
                       std::string(RESOURCES_PATH) + "exit.png");
    buttons["login"] = &loginButton;

    Entry loginEntry({float(screenRes.width) * 0.2f, float(screenRes.height) * 0.4f}, 12, &window, 24);
    entries["login"] = &loginEntry;
    Entry passwordEntry({float(screenRes.width) * 0.2f, float(screenRes.height) * 0.5f}, 12, &window, 24);
    entries["password"] = &passwordEntry;
    Event event{};
    Text loginText, passwordText;
    Font arial;
    arial.loadFromFile(std::string(RESOURCES_PATH) + "arialmt.ttf");
    loginText.setString("Login:");
    loginText.setPosition(float(screenRes.width) * 0.2f + 10, float(screenRes.height) * 0.4f - 30);
    loginText.setFont(arial);
    loginText.setFillColor(Color::Black);
    loginText.setCharacterSize(24);

    passwordText.setString("Password:");
    passwordText.setPosition(float(screenRes.width) * 0.2f + 10, float(screenRes.height) * 0.5f - 30);
    passwordText.setFont(arial);
    passwordText.setFillColor(Color::Black);
    passwordText.setCharacterSize(24);

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            exitButton.eventCheck(event);
            switch (userStatus) {
                case LOGIN:
                    loginEntry.eventCheck(event);
                    passwordEntry.eventCheck(event);
                    loginButton.eventCheck(event);
                    break;
            }
        }
        window.draw(spriteBackground);
        window.draw(spriteBattleship);
        exitButton.draw();
        switch (userStatus) {
            case LOGIN:
                loginEntry.draw();
                window.draw(loginText);
                passwordEntry.draw();
                window.draw(passwordText);
                loginButton.draw();
                break;
        }
        spriteCursor.setPosition(float(Mouse::getPosition().x), float(Mouse::getPosition().y));
        window.draw(spriteCursor);
        window.display();
    }

    return 0;
}