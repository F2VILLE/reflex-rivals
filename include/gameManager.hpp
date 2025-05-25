#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP
#include <PINOUTS.hpp>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
#include <ESP32Servo.h>
#define COLOR_INDEX_R 0
#define COLOR_INDEX_W 1
#define COLOR_INDEX_B 2

class Player
{
public:
    Player();
    ~Player();
    int score;
    void increaseScore(int points);
    void decreaseScore(int points);
    void resetScore();
    void displayScore();
};

struct GameSettings
{
    int maxScore;
    int maxTimeout;
    int buzzerVolume;
};


class GameManager
{
public:
    GameManager();
    ~GameManager();

    GameSettings settings;
    int colors[3][3];
    int currentColor[3];
    int currentColorIndex;
    Player player1;
    Player player2;
    int running = 0;
    int ledState = 0;
    void startGame();
    void endGame(Adafruit_SSD1306 &display, Servo &motor, Adafruit_NeoPixel &WS2812B);
    void resetGame();
    void turnOnLED(int color[3]);
    void turnOffLED();
    int getRandomColorIndex();
    void setColor(int colorIndex);
};


#endif 