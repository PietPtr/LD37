#include <SFML/Graphics.hpp>
#include <iostream>
#include "Audio.h"
#include "Circle.h"

using namespace sf;

class Game
{
    public:
        Game(RenderWindow* window);
        void initialize();
        void update();
        void draw();
        bool isWindowOpen();

        void loadAudio(std::vector<std::string> fileNames);
        void loadTextures(std::vector<std::string> fileNames);

        int randint(int low, int high, int seed);
        int randint(int low, int high);

        std::array<unsigned char, 8> generateDNA();

        void breed(Circle dad, Circle mom);
    protected:
    private:
        RenderWindow* window;
        View view;

        Time dt;
        Time totalTime;
        Clock clock;
        int frame = 0;

        std::vector<std::string> audioFileNames { };
        std::vector<std::string> textureFileNames { "overlay.png", "ground.png" };

        std::vector<Audio*> sfx;
        std::vector<Texture> textures;

        std::vector<Circle> circles;
};
