#ifndef CIRCLE_H
#define CIRCLE_H
#include <SFML/Graphics.hpp>

using namespace sf;

class Circle
{
    public:
        Circle(Vector2f pos, std::array<unsigned char, 8> momDNA,
                             std::array<unsigned char, 8> dadDNA);
        void update(double dt);
        void draw(RenderWindow* window);
        void moveTo(Vector2f goalPos) { this->goalPos = goalPos; };
    protected:
    private:
        unsigned char r()      { return dadDNA[0]; };
        unsigned char g()      { return momDNA[1]; };
        unsigned char b()      { return momDNA[2]; };
        unsigned char radius() { return dadDNA[3]; };
        unsigned char speed()  { return dadDNA[4]; };
        std::array<unsigned char, 8> momDNA{ {0, 0, 0, 0, 0, 0, 0, 0} };
        std::array<unsigned char, 8> dadDNA{ {0, 0, 0, 0, 0, 0, 0, 0} };
        Vector2f pos;
        Vector2f goalPos {-1, -1};
};

#endif // CIRCLE_H
