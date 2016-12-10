#include <SFML/Graphics.hpp>
#include "Game.h"
#include <bitset>

using namespace sf;

Game::Game(RenderWindow* window)
{
    this->window = window;
}

void Game::initialize()
{
    unsigned char test = 255;
    std::bitset<8> x(test);
    std::cout << x << "\n";
    test = (1 << 1) ^ test;
    std::bitset<8> y(test);
    std::cout << y << "\n";

    loadAudio(audioFileNames);
    loadTextures(textureFileNames);

    for (int i = 0; i < 4; i++)
    {
        Vector2f pos = Vector2f(20 + i/2 * 320, 10 + i%2 * 140);
        Circle circle = Circle(pos, generateDNA(), generateDNA());
        circle.setAge(20);
        circles.push_back(circle);
    }

    goalCircle = new Circle(Vector2f(496, 282), generateDNA(), generateDNA());
    goalCircle->setAge(100);
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

    if (Mouse::isButtonPressed(Mouse::Left))
    {
        Vector2i mousePos = Vector2i(Mouse::getPosition().x - window->getPosition().x,
                                     Mouse::getPosition().y - window->getPosition().y);

        Vector2f clickPos = window->mapPixelToCoords(mousePos);

        for (int i = 0; i < circles.size(); i++)
        {
            float dist = sqrt(pow(clickPos.x - 96 - circles[i].getPos().x, 2) +
                              pow(clickPos.y - 32 - circles[i].getPos().y, 2));

            if (dist < circles[i].getRadius())
            {
                circles[i].setDragged(true);
                draggedCircle = i;
                break;
            }
        }
    }
    else
    {
        draggedCircle = -1;
    }

    for (int i = circles.size() - 1; i >= 0; i--)
    {
        // Make sure only one circle is dragged
        if (i != draggedCircle)
            circles[i].setDragged(false);

        // Set new places for circles to go to
        if (!(circles[i].isMoving()))
        {
            float radius = circles[i].getRadius();
            Vector2f goalPos = Vector2f(randint(radius, 448 - radius),
                                        randint(radius, 224 - radius));

            circles[i].moveTo(goalPos);
        }

        // Find a mate
        for (int j = 0; j < circles.size(); j++)
        {
            if (i != j && !(circles[i].isBreeding()) && circles[i].getAge() > 5 &&
                          !(circles[j].isBreeding()) && circles[j].getAge() > 5 &&
                          !(circles[i].isDragged()) && !(circles[j].isDragged()) &&
                          circles.size() < 12)
            {
                float dist = sqrt(pow(circles[i].getPos().x - circles[j].getPos().x, 2) +
                                  pow(circles[i].getPos().y - circles[j].getPos().y, 2));

                if (dist < circles[i].getRadius() + circles[j].getRadius())
                {
                    breed(i, j);
                }
            }
        }

        // Let the circle update itself
        if (circles[i].update(dt.asSeconds(), window) < 0)
        {
            circles.erase(circles.begin() + i);
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

    Sprite tray;
    tray.setTexture(textures[2]);
    tray.setPosition(trayPosition);
    window->draw(tray);

    for (int i = 0; i < circles.size(); i++)
    {
        circles[i].draw(window);
    }

    Sprite overlay;
    overlay.setTexture(textures[0]);
    overlay.setPosition(Vector2f(0,0));
    window->draw(overlay);

    if (draggedCircle > -1)
    {
        circles[draggedCircle].draw(window);
    }

    goalCircle->draw(window);

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

void Game::breed(int dad, int mom)
{
    std::array<unsigned char, 8> dadDNA;
    std::array<unsigned char, 8> momDNA;

    if (randint(0, 100) > 50) { dadDNA = circles[dad].getDadDNA(); }
    else { dadDNA = circles[dad].getMomDNA(); }

    if (randint(0, 100) > 50) { momDNA = circles[mom].getDadDNA(); }
    else { momDNA = circles[mom].getMomDNA(); }

    Vector2f pos = Vector2f((circles[dad].getPos().x + circles[mom].getPos().x) / 2,
                            (circles[dad].getPos().y + circles[mom].getPos().y) / 2);

    circles[dad].setBreeding(true);
    circles[mom].setBreeding(true);

    Circle circle = Circle(pos, momDNA, dadDNA);

    circles.push_back(circle);
}


std::array<unsigned char, 8> Game::pointMutation(std::array<unsigned char, 8> source)
{
    // take a random gene and flip a random bit
    std::array<unsigned char, 8> product = source;
    int gene = randint(0, 7);

    product[gene] = (1 << randint(0, 7)) ^ source[gene];
    return product;
}

std::array<unsigned char, 8> Game::shiftMutation(std::array<unsigned char, 8> source)
{
    // take a random gene and shift the byte 1 bit to the left
    std::array<unsigned char, 8> product = source;
    int gene = randint(0, 7);
    product[gene] = product[gene] << 1;
    return product;
}

std::array<unsigned char, 8> Game::reverseMutation(std::array<unsigned char, 8> source)
{
    // take a random gene and reverse the bits
    int gene = randint(0, 7);
    unsigned char original = source[gene];

    std::array<unsigned char, 8> product = source;
    product[gene] = 0;

    for (int i = 0; i < 8; i++)
         product[gene] |= ((original >> i) & 0b1) << (7 - i);

    return product;
}

std::array<unsigned char, 8> Game::swapMutation(std::array<unsigned char, 8> source)
{
    // take two random genes and swap them
    int gene1 = randint(0, 7);
    int gene2 = randint(0, 7);

    std::array<unsigned char, 8> product = source;
    product[gene1] = source[gene2];
    product[gene2] = source[gene2];

    return product;
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
