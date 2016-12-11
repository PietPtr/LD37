#include "Circle.h"
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;

Circle::Circle(Vector2f pos, std::array<unsigned char, 8> momDNA,
                             std::array<unsigned char, 8> dadDNA)
{
    this->pos = pos;
    this->momDNA = momDNA;
    this->dadDNA = dadDNA;

    this->r = dadDNA[0];
    this->g = momDNA[1];
    this->b = momDNA[2];
    this->radius = dadDNA[3] / 12 + 6;
    this->speed = dadDNA[4] / 10 + 5;

    this->age = 0;
}

int Circle::update(double dt, RenderWindow* window)
{
    if (goalPos != Vector2f(-1, -1) && !dragged && killed < 0 && !sent)
    {
        float distToTrash = sqrt(pow(pos.x - 598 + 96, 2) + pow(pos.y - 96 + 32, 2));

        if (distToTrash < 32)
            killed = age;

        float distToTray = sqrt(pow(pos.x - 44 + 96, 2) + pow(pos.y - 96 + 32, 2));

        if (distToTray < 32)
            sent = true;

        Vector2f dir = pos - goalPos;
        float dirMag = -sqrt(pow(dir.x, 2) + pow(dir.y, 2));

        if (dirMag < -4)
        {
            Vector2f dirUnit = Vector2f(dir.x / dirMag, dir.y / dirMag);
            this->pos += Vector2f(dirUnit.x * dt * speed, dirUnit.y * dt * speed);
            moving = true;
        }
        else
        {
            moving = false;
        }
    }
    else if (dragged)
    {
        Vector2i mousePos = Vector2i(Mouse::getPosition().x - windowPos.x,
                                     Mouse::getPosition().y - windowPos.y);

        Vector2f clickPos = window->mapPixelToCoords(mousePos);

        clickPos.x = clickPos.x > 640 ? 640 : clickPos.x;
        clickPos.y = clickPos.y > 250 ? 250 : clickPos.y;
        clickPos.x = clickPos.x < 0 ? 0 : clickPos.x;
        clickPos.y = clickPos.y < 0 ? 0 : clickPos.y;

        this->pos = Vector2f(clickPos.x - 96, clickPos.y - 32);
    }
    else if (killed >= 0)
    {
        return -1;
    }
    else if (sent)
    {
        return 1;
    }

    age += dt;

    return 0;
}

void Circle::draw(RenderWindow* window)
{
    CircleShape circle;
    circle.setRadius(radius);
    circle.setFillColor(Color(r, g, b));
    circle.setOutlineColor(Color::Black);
    circle.setOutlineThickness(0.5);
    circle.setPosition(pos + Vector2f(96, 32));
    circle.setOrigin(radius, radius);

    if (isBreeding())
        circle.setFillColor(Color(r, g, b, 100));

    if (age < 5)
        circle.setFillColor(Color(100, 100, 100, 100));

    window->draw(circle);

    this->windowPos = window->getPosition();
}

