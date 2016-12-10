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
        Vector2f pos = Vector2f(20 + i/2 * 320, 10 + i%2 * 140);
        Circle circle = Circle(pos, generateDNA(), generateDNA());
        circles.push_back(circle);
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

    for (int i = 0; i < 4; i++)
    {
        if (!(circles[i].isMoving()))
        {
            float radius = circles[i].radius();
            Vector2f goalPos = Vector2f(randint(radius, 448 - radius),
                                        randint(radius, 224 - radius));

            circles[i].moveTo(goalPos);
        }
    }

    for (int i = 0; i < circles.size(); i++)
    {
        circles[i].update(dt.asSeconds());

        for (int j = 0; j < circles.size(); j++)
        {
            if (i != j)
            {
                float dist = sqrt(pow(circles[i].getPos().x - circles[j].getPos().x, 2) +
                                  pow(circles[i].getPos().y - circles[j].getPos().y, 2));

                if (dist < circles[i].radius() + circles[j].radius())
                {
                    std::cout << "Close encounter between " << i << " and " << j <<
                                 " at distance of " << dist << "\n";
                }
            }
        }
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

std::array<unsigned char, 8> Game::generateDNA()
{
    std::array<unsigned char, 8> DNA;

    for (int i = 0; i < DNA.size(); i++)
    {
        DNA[i] = randint(0, 255);
    }
    return DNA;
}
