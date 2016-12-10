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
}

void Circle::update(double dt)
{
    if (goalPos != Vector2f(-1, -1))
    {
        Vector2f dir = -pos - goalPos;
        float dirMag = sqrt(pow(dir.x, 2) + pow(dir.y, 2));

        if (dirMag > 4)
        {
            Vector2f dirUnit = Vector2f(dir.x / dirMag, dir.y / dirMag);
            this->pos += Vector2f(dirUnit.x * dt * speed(), dirUnit.y * dt * speed());
        }
    }
}

void Circle::draw(RenderWindow* window)
{
    CircleShape circle;
    circle.setRadius(radius() / 8 + 4.5);
    circle.setFillColor(Color::Black);
    circle.setPosition(pos + Vector2f(95.5, 31.5));
    window->draw(circle);

    circle.setRadius(radius() / 8 + 4);
    circle.setFillColor(Color(r(), g(), b()));
    circle.setPosition(pos + Vector2f(96, 32));
    window->draw(circle);

}