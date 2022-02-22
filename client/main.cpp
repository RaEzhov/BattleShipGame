#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;

const unsigned int FRAMERATE = 30;

int main()
{

    auto screenRes = sf::VideoMode::getFullscreenModes();
    // Объект, который, собственно, является главным окном приложения
    RenderWindow window(screenRes[0], "Battleship",sf::Style::Fullscreen);
    // Загрузка текстур из памяти
    Texture background, battleshipText, button[2];//, field;
    background.loadFromFile("./resources/mainMenu.jpg");
    //field.loadFromFile("./resources/Field1.bmp");
    battleshipText.loadFromFile("./resources/battleship.png");
    button[0].loadFromFile("./resources/button1.png");
    button[1].loadFromFile("./resources/button2.png");
    // Получение разрешения фона (1920х1080)
    unsigned int width = background.getSize().x,
                 height = background.getSize().y;

    // Объявление переменных спрайтов
    Sprite spriteBackground(background), spriteBattleship(battleshipText), spriteButton0(button[0]), spriteButton1(button[1]);//, spriteField(field);
    spriteBackground.scale(width/screenRes[0].width, height/screenRes[0].height);
    //spriteField.scale(2, 2);
    //spriteField.setPosition({100, 100});
    spriteBattleship.setPosition(width*0.35, height*0.85);

    spriteButton0.setPosition(width*0.1, height*0.4);
    spriteButton1.setPosition(width*0.1, height*0.4);

    window.setFramerateLimit(FRAMERATE);
    // Главный цикл приложения. Выполняется, пока открыто окно
    int pressed = 0, menu = 0;
    while (window.isOpen())
    {
        // Обрабатываем очередь событий в цикле
        Event event;
        while (window.pollEvent(event))
        {
            // Пользователь нажал на «крестик» и хочет закрыть окно?
            if (event.type == Event::Closed)
                // тогда закрываем его
                window.close();
            if (IntRect(spriteButton0.getPosition().x, spriteButton0.getPosition().y,
                        button[0].getSize().x, button[0].getSize().y).contains(Mouse::getPosition(window))) {
                spriteButton0.setColor(Color::White);
                menu = 1;
            } else {
                spriteButton0.setColor(Color(225,225,225,255));
                menu = 0;
            }
            if (Mouse::isButtonPressed(Mouse::Left) && menu == 1) {
                pressed = 1;
            } else {
                pressed = 0;
            }
        }

        window.draw(spriteBackground);
        window.draw(spriteBattleship);
        if (pressed == 0){
            window.draw(spriteButton0);
        } else {
            window.draw(spriteButton1);
            window.close();
        }
        //window.draw(spriteField);
        // Отрисовка окна
        window.display();
    }

    return 0;
}