#include <Arduino.h>
#include <gameManager.hpp>



Player::Player() {
    score = 0;
}

Player::~Player() {
    // Destructor logic if needed
}

void Player::increaseScore(int points) {
    score += points;
}

void Player::decreaseScore(int points) {
    score -= points;
}

void Player::resetScore() {
    score = 0;
}

void Player::displayScore() {
    Serial.print("Player Score: ");
    Serial.println(score);
}

GameManager::GameManager() {
    settings.maxScore = 100;
    settings.maxTimeout = 60;
    colors[0][0] = 255; colors[0][1] = 0;   colors[0][2] = 0;   // Red
    colors[1][0] = 0;   colors[1][1] = 255; colors[1][2] = 255;   // White
    colors[2][0] = 0;   colors[2][1] = 0;   colors[2][2] = 255; // Blue

    currentColor[0] = 0;
    currentColor[1] = 0;
    currentColor[2] = 0;

    player1 = Player();
    player2 = Player();

    running = 0;
    ledState = 0;

}

int GameManager::getRandomColorIndex() {
    int randomIndex = random(0, 3);
    return randomIndex;
}

void GameManager::setColor(int colorIndex) {
    for (int i = 0; i < 3; i++) {
        currentColor[i] = colors[colorIndex][i];
    }
    currentColorIndex = colorIndex;
}


void GameManager::startGame() {
    Serial.println("Game Started");
    player1.resetScore();
    player2.resetScore();
    running = 1;
}



void GameManager::endGame() {
    running = 0;
    turnOffLED();
    Serial.println("Game Ended");
    if (player1.score > player2.score) {
        Serial.println("Player 1 Wins!");
    } else if (player2.score > player1.score) {
        Serial.println("Player 2 Wins!");
    } else {
        Serial.println("It's a Tie!");
    }
}

void GameManager::resetGame() {
    running = 0;
    turnOffLED();
    Serial.println("Game Reset");
    player1.resetScore();
    player2.resetScore();
}

void GameManager::turnOnLED(int* color) {
    ledState = 1;
    Serial.print("LED Color: ");
    Serial.print(color[0]);
    Serial.print(", ");
    Serial.print(color[1]);
    Serial.print(", ");
    Serial.println(color[2]);
    analogWrite(LED_R, color[0]);
    analogWrite(LED_G, color[1]);
    analogWrite(LED_B, color[2]);
}

void GameManager::turnOffLED() {
    ledState = 0;
    analogWrite(LED_R, 0);
    analogWrite(LED_G, 0);
    analogWrite(LED_B, 0);
}

GameManager::~GameManager() {
}