#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <string>
#include <iostream>
#include <memory>

#include "screen_objects.h"


enum status{
    LOGIN = 1,
    MAIN_MENU = 2,
    IN_GAME = 3,
    IN_FRIENDS_MENU = 4
};

static unsigned int userStatus = LOGIN;

std::unordered_map<std::string, std::unique_ptr<Button>> buttons;
std::unordered_map<std::string, std::unique_ptr<Entry>> entries;

static std::unique_ptr<sf::TcpSocket> server;

void loginFnc(){
    auto login = entries["login"]->getStr();
    auto password = entries["password"]->getStr();
    Packet packet;
    packet << login << password;
    server->send(packet);
    packet.clear();
    bool authDone;
    server->receive(packet);
    packet >> authDone;
    if (authDone){
        userStatus = MAIN_MENU;
    }
}

int main() {
    server = std::make_unique<sf::TcpSocket>();
    sf::Socket::Status status = server->connect(IP_ADDR, PORT);
    if (status != sf::Socket::Done)
    {
        std::cout << "Connection error";
        return 1;
    }

    auto screenRes = sf::VideoMode::getDesktopMode();
    auto window = std::make_shared<RenderWindow>(screenRes, "Battleship", Style::Fullscreen);
    window->setFramerateLimit(FRAMERATE);
    window->setMouseCursorVisible(false);

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

    buttons["exit"] = std::make_unique<Button>(float(screenRes.width) * 0.05f, float(screenRes.height) * 0.85f, scaleWindow, nullptr, window,
                                               std::string(RESOURCES_PATH) + "exit.png");

    buttons["login"] = std::make_unique<Button>(float(screenRes.width) * 0.22f, float(screenRes.height) * 0.6f, scaleWindow * 0.5f, loginFnc, window,
                                                std::string(RESOURCES_PATH) + "exit.png");

    entries["login"] = std::make_unique<Entry>(Vector2{float(screenRes.width) * 0.2f, float(screenRes.height) * 0.4f}, 12,
                                               window, 24);

    entries["password"] = std::make_unique<Entry>(Vector2{float(screenRes.width) * 0.2f, float(screenRes.height) * 0.5f}, 12,
                                                  window, 24);

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

    while (window->isOpen()) {
        while (window->pollEvent(event)) {
            if (event.type == Event::Closed)
                window->close();
            buttons["exit"]->eventCheck(event);
            switch (userStatus) {
                case LOGIN:
                    entries["login"]->eventCheck(event);
                    entries["password"]->eventCheck(event);
                    buttons["login"]->eventCheck(event);
                    break;
            }
        }
        window->draw(spriteBackground);
        window->draw(spriteBattleship);
        buttons["exit"]->draw();
        switch (userStatus) {
            case LOGIN:
                entries["login"]->draw();
                window->draw(loginText);
                entries["password"]->draw();
                window->draw(passwordText);
                buttons["login"]->draw();
                break;
        }
        spriteCursor.setPosition(static_cast<float>(Mouse::getPosition().x), static_cast<float>(Mouse::getPosition().y));
        window->draw(spriteCursor);
        window->display();
    }
    return 0;
}