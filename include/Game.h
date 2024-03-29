#include <SFML/Graphics.hpp>
#include <iostream>
#include "Audio.h"
#include "Circle.h"

using namespace sf;

enum Result {
    NONE,
    BAD,
    OKAY,
    NICE,
    GREAT
};

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

        std::array<unsigned char, 8> pointMutation(std::array<unsigned char, 8> source);
        std::array<unsigned char, 8> shiftMutation(std::array<unsigned char, 8> source);
        std::array<unsigned char, 8> reverseMutation(std::array<unsigned char, 8> source);
        std::array<unsigned char, 8> swapMutation(std::array<unsigned char, 8> source);

        std::array<unsigned char, 8> breedMutation(std::array<unsigned char, 8> source);

        void breed(int dad, int mom);

        void irradiate(int circleIndex);

        void drawNumbers(RenderWindow* window, int number, Vector2f position);
    protected:
    private:
        RenderWindow* window;
        View view;

        Time dt;
        Time totalTime;
        Clock clock;
        int frame = 0;

        std::vector<std::string> audioFileNames { };
        std::vector<std::string> textureFileNames { "overlay.png", "ground.png", "tray.png",
                                                    "slider.png", "bad.png", "okay.png",
                                                    "nice.png", "great.png", "numbers.png",
                                                    "tutorial.png"};

        std::vector<Audio*> sfx;
        std::vector<Texture> textures;

        std::vector<Circle> circles;

        int draggedCircle = -1;
        int sliderx = 0;
        int radiation = 0;

        float trayy/*lmao*/ = -16;
        float sendy = 0;
        float traySpeed = 280;

        Vector2f clickPos {0, 0};

        int filledTray = -1;

        int score = 0;

        bool tutorial = true;

        Result result = NONE;
        Time resultTime;

        Circle* goalCircle;
};
