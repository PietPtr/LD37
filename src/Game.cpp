#include <SFML/Graphics.hpp>
#include "Game.h"

using namespace sf;

Game::Game(RenderWindow* window)
{
    this->window = window;
}

void Game::initialize()
{
    loadAudio(audioFileNames);
    loadTextures(textureFileNames);

    for (int i = 0; i < 4; i++)
    {
        circles.push_back(generateCircle());
    }
}

void Game::update()
{
    Event event;
    while (window->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window->close();
        if (event.type == Event::KeyPressed)
        {
            if (event.key.code == Keyboard::Escape)
            {
                window->close();
            }
        }
    }

    dt = clock.restart();
    totalTime += dt;

    for (int i = 0; i < circles.size(); i++)
    {
        circles[i].update(dt.asSeconds());
    }

    frame++;
}

void Game::draw()
{
    window->clear();

    view.setSize(Vector2f(640, 360));
    view.setCenter(Vector2f(320, 180));
    window->setView(view);

    Sprite ground;
    ground.setTexture(textures[1]);
    ground.setPosition(Vector2f(96, 32));
    window->draw(ground);

    for (int i = 0; i < circles.size(); i++)
    {
        circles[i].draw(window);
    }

    Sprite overlay;
    overlay.setTexture(textures[0]);
    overlay.setPosition(Vector2f(0,0));
    window->draw(overlay);


    window->display();
}

bool Game::isWindowOpen()
{
    return window->isOpen();
}

void Game::loadAudio(std::vector<std::string> audioFileNames)
{
    for (int i = 0; i < audioFileNames.size(); i++)
    {
        sfx.push_back(new Audio());
        sfx.back()->init(audioFileNames[i]);
    }
}

void Game::loadTextures(std::vector<std::string> textureFileNames)
{
    for (int i = 0; i < textureFileNames.size(); i++)
    {
        Texture texture;
        if (!texture.loadFromFile("resources/textures/" + textureFileNames.at(i)))
            window->close();
        else
            std::cout << "loaded resources/textures/" << textureFileNames.at(i) << "\n";
        textures.push_back(texture);
    }
}

int Game::randint(int low, int high)
{
    int value = rand() % (high + 1 - low) + low;
    //srand(totalTime.asMicroseconds() * value * rand());

    return value;
}

Circle Game::generateCircle()
{
    Vector2f pos = Vector2f(randint(0, 448), randint(0, 224));
    std::array<unsigned char, 8> momDNA;
    std::array<unsigned char, 8> dadDNA;

    for (int i = 0; i < momDNA.size(); i++)
    {
        momDNA[i] = randint(0, 255);
        dadDNA[i] = randint(0, 255);
    }
    dadDNA[4] = 25;
    Circle circle = Circle(pos, momDNA, dadDNA);
    return circle;
}
