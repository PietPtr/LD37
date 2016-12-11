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
        if (event.type == Event::MouseButtonPressed)
        {
            tutorial = false;
        }
    }

    dt = clock.restart();
    totalTime += dt;

    // Determine what circle the player tries to drag
    if (Mouse::isButtonPressed(Mouse::Left))
    {
        Vector2i mousePos = Vector2i(Mouse::getPosition().x - window->getPosition().x,
                                         Mouse::getPosition().y - window->getPosition().y);

        clickPos = window->mapPixelToCoords(mousePos);

        if (clickPos.y < 292)
        {
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
        else if (clickPos.y >= 292 && clickPos.x > 98 && clickPos.x < 254)
        {
            sliderx = clickPos.x - 98;
            radiation = (sliderx / 155.0) * 100;
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
        int updateReturn = circles[i].update(dt.asSeconds(), window);
        if (updateReturn == -1)
        {
            circles.erase(circles.begin() + i);
        }
        else if (updateReturn == 1)
        {
            filledTray = i;
            sendy = clickPos.y;
        }

        // Irradiate the circles.
        irradiate(i);

    }

    if (filledTray >= 0 && trayy > -156)
    {
        circles[filledTray].setPos(Vector2f(circles[filledTray].getPos().x,
                                            trayy + sendy - 16));
        trayy -= dt.asSeconds() * traySpeed;
    }
    else if (filledTray >= 0 && trayy <= -156)
    {
        int scoreAdd = circles[filledTray].compareDNA(goalCircle);
        if (scoreAdd <= 100)
        {
            result = GREAT;
            score += 100;
        }
        else if (scoreAdd > 100 && scoreAdd <= 200)
        {
            result = NICE;
            score += 10;
        }
        else if (scoreAdd > 200 && scoreAdd <= 300)
        {
            result = OKAY;
            score += 1;
        }
        else
        {
            result = BAD;
            score += -10;
        }

        resultTime = totalTime;

        circles[filledTray].kill();
        filledTray = -1;
    }
    else if (filledTray <= 0 && trayy < -16)
    {
        trayy += dt.asSeconds() * traySpeed;
    }
    else
    {
        trayy = -16;
    }

    score = score <= 0 ? 0 : score;

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
    tray.setPosition(Vector2f(12, trayy));
    window->draw(tray);

    for (int i = 0; i < circles.size(); i++)
    {
        circles[i].draw(window);
    }

    RectangleShape rad;
    rad.setFillColor(Color(0, 255, 0, radiation));
    rad.setPosition(Vector2f(96, 31));
    rad.setSize(Vector2f(448, 224));
    window->draw(rad);

    Sprite overlay;
    overlay.setTexture(textures[0]);
    overlay.setPosition(Vector2f(0,0));
    window->draw(overlay);

    Sprite slider;
    slider.setTexture(textures[3]);
    slider.setPosition(Vector2f(98 + sliderx, 292));
    window->draw(slider);

    if (draggedCircle > -1)
    {
        circles[draggedCircle].draw(window);
    }

    if (result != NONE && totalTime.asSeconds() - resultTime.asSeconds() < 1)
    {
        Sprite resultSprite;
        resultSprite.setPosition(Vector2f(5, 148));
        resultSprite.setTexture(textures[3 + (int)result]);
        window->draw(resultSprite);
    }

    drawNumbers(window, score, Vector2f(40 - 5 * (int)(log10(score)), 232));

    goalCircle->draw(window);

    if (totalTime.asSeconds() < 10 && tutorial)
    {
        Sprite tut;
        tut.setTexture(textures[9]);
        tut.setPosition(0,0);
        window->draw(tut);
    }

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
    // Inherit DNA
    std::array<unsigned char, 8> dadDNA;
    std::array<unsigned char, 8> momDNA;

    if (randint(1, 100) > 50) { dadDNA = circles[dad].getDadDNA(); }
    else { dadDNA = circles[dad].getMomDNA(); }

    if (randint(1, 100) > 50) { momDNA = circles[mom].getDadDNA(); }
    else { momDNA = circles[mom].getMomDNA(); }

    // Set the position
    Vector2f pos = Vector2f((circles[dad].getPos().x + circles[mom].getPos().x) / 2,
                            (circles[dad].getPos().y + circles[mom].getPos().y) / 2);

    // Mutate, not based on radiation.
    dadDNA = breedMutation(dadDNA);
    momDNA = breedMutation(momDNA);

    circles[dad].setBreeding(true);
    circles[mom].setBreeding(true);

    Circle circle = Circle(pos, momDNA, dadDNA);

    circles.push_back(circle);
}

void Game::irradiate(int c)
{
    std::array<unsigned char, 8> newDNA;
    int strand = randint(1, 2);
    if (strand == 1)
        newDNA = circles[c].getMomDNA();
    else
        newDNA = circles[c].getDadDNA();

    if (randint(1, 10000) < radiation * 5 - 5)
        newDNA = pointMutation(newDNA);

    if (strand == 1)
        circles[c].setMomDNA(newDNA);
    else
        circles[c].setDadDNA(newDNA);

}

std::array<unsigned char, 8> Game::breedMutation(std::array<unsigned char, 8> source)
{
    std::array<unsigned char, 8> product = source;
    if (randint(1, 100) <= 25)
        product = pointMutation(product);
    if (randint(1, 100) <= 10)
        product = shiftMutation(product);
    if (randint(1, 100) <= 5)
        product = reverseMutation(product);
    if (randint(1, 100) <= 1)
        product = swapMutation(product);

    return product;
}

std::array<unsigned char, 8> Game::pointMutation(std::array<unsigned char, 8> source)
{
    //std::cout << "Point mutation!\n";
    // take a random gene and flip a random bit
    std::array<unsigned char, 8> product = source;
    int gene = randint(0, 7);

    product[gene] = (1 << randint(0, 7)) ^ source[gene];
    return product;
}

std::array<unsigned char, 8> Game::shiftMutation(std::array<unsigned char, 8> source)
{
    //std::cout << "Shift mutation!\n";

    // take a random gene and shift the byte 1 bit to the left
    std::array<unsigned char, 8> product = source;
    int gene = randint(0, 7);
    product[gene] = product[gene] << 1;
    return product;
}

std::array<unsigned char, 8> Game::reverseMutation(std::array<unsigned char, 8> source)
{
    //std::cout << "Reverse mutation!\n";

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
    //std::cout << "Swap mutation!\n";

    // take two random genes and swap them
    int gene1 = randint(0, 7);
    int gene2 = randint(0, 7);

    std::array<unsigned char, 8> product = source;
    product[gene1] = source[gene2];
    product[gene2] = source[gene1];

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

std::string DNAstring(std::array<unsigned char, 8> DNA)
{
    for (int i = 0; i < 8; i++)
    {
        std::bitset<8> x(DNA[i]);
        std::cout << x << " ";
    }
    std::cout << "\n";
}

void Game::drawNumbers(RenderWindow* window, int number, Vector2f position)
{
    std::string text = std::to_string(number);

    int drawX = position.x;
    int drawY = position.y;

    for (int i = 0; i < text.length(); i++)
    {
        int num = (int)(text[i]);

        Sprite charSprite;
        charSprite.setTexture(textures[8]);

        charSprite.setTextureRect(IntRect( (num - 48) * 10, 0, 10, 13));

        charSprite.setPosition(Vector2f(drawX, drawY));

        window->draw(charSprite);

        drawX += 10;
    }
}



