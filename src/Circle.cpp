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

void Circle::update(double dt)
{
    if (goalPos != Vector2f(-1, -1))
    {
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

    age += dt;
}

void Circle::draw(RenderWindow* window)
{
    CircleShape circle;
    circle.setRadius(radius + 0.5);
    circle.setFillColor(Color::Black);
    circle.setPosition(pos + Vector2f(95.5, 31.5));
    circle.setOrigin(radius, radius);
    window->draw(circle);

    circle.setRadius(radius);
    circle.setFillColor(Color(r, g, b));
    circle.setPosition(pos + Vector2f(96, 32));
    circle.setOrigin(radius, radius);

    if (age < 5)
    {
        circle.setFillColor(Color(100, 100, 100, 100));
    }

    window->draw(circle);

}
