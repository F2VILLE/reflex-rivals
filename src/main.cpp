#include <Arduino.h>
#include <gameManager.hpp>
#include <time.h>

GameManager gameManager;
time_t lastBlink;

void setup()
{
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(PLAYER1_BUTTON_W, INPUT_PULLUP);
  pinMode(PLAYER1_BUTTON_R, INPUT_PULLUP);
  pinMode(PLAYER1_BUTTON_B, INPUT_PULLUP);
  pinMode(PLAYER2_BUTTON_W, INPUT_PULLUP);
  pinMode(PLAYER2_BUTTON_R, INPUT_PULLUP);
  pinMode(PLAYER2_BUTTON_B, INPUT_PULLUP);
  pinMode(START_BUTTON, INPUT_PULLUP);

  Serial.begin(9600);
  delay(2000);
  Serial.println("");
  gameManager = GameManager();
  gameManager.settings.buzzerVolume = 0;
  gameManager.settings.maxTimeout = 3000;
  Serial.println("Game Manager Initialized");
}

int playerButtonRead()
{
  int player1Button = digitalRead(PLAYER1_BUTTON_W) ? 0 : 0x10;
  if (digitalRead(PLAYER1_BUTTON_W) == LOW || digitalRead(PLAYER2_BUTTON_W) == LOW)
  {
    return 0x01;
  }
  if (digitalRead(PLAYER1_BUTTON_R) == LOW || digitalRead(PLAYER2_BUTTON_R) == LOW)
  {
    return 0x02;
  }
  if (digitalRead(PLAYER1_BUTTON_B) == LOW || digitalRead(PLAYER2_BUTTON_B) == LOW)
  {
    return 0x03;
  }
  return 0;
}

void loop()
{
  // Serial.println(digitalRead(START_BUTTON));
  if (digitalRead(START_BUTTON) == LOW && gameManager.running == 0)
  {
    Serial.println("Button pressed");
    gameManager.startGame();
    delay(100); // Debounce delay
  }

  int playerButton = playerButtonRead();
  if (playerButton != 0 && gameManager.running == 1)
  {
    int player = (playerButton & 0x0F);
    int color = (playerButton & 0xF0) >> 4;
    Serial.print("Player ");
    Serial.print(player);
    Serial.print(" pressed button ");
    Serial.println(color);
    if (gameManager.currentColorIndex == color)
    {
      Serial.println("Correct!");
      if (player == 1)
      {
        gameManager.player1.increaseScore(1);
      }
      else
      {
        gameManager.player2.increaseScore(1);
      }
    }
    else
    {
      Serial.println("Wrong!");
      if (player == 1)
      {
        gameManager.player1.decreaseScore(1);
      }
      else
      {
        gameManager.player2.decreaseScore(1);
      }
    }
    gameManager.player1.displayScore();
    gameManager.player2.displayScore();
  }

  if (gameManager.running == 1)
  {
    if (!gameManager.ledState) {
    int delayTime = random(50, gameManager.settings.maxTimeout);
    Serial.print("Random delay: ");
    Serial.println(delayTime);
    gameManager.setColor(gameManager.colors[gameManager.getRandomColorIndex()]);

    delay(delayTime);
    gameManager.turnOnLED(gameManager.currentColor);
    analogWrite(BUZZER, gameManager.settings.buzzerVolume); // Set buzzer to low volume (range: 0-255)
    analogWrite(BUZZER, 0);                                 // Turn off buzzer
    lastBlink = millis();
    }
    else {
      if (millis() - lastBlink > 500) {
        gameManager.turnOffLED();
      }
    }
  }
}
