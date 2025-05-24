#include <Arduino.h>
#include <gameManager.hpp>
#include <time.h>

GameManager gameManager;
time_t lastBlink;
int waitToTurnOn = 0;
int delayTime = 0;
int coolDown = 0;
time_t coolDownTime;

time_t lastButtonsPressed[6] = {0, 0, 0, 0, 0, 0};

void setup()
{
  Serial.begin(115200);

  Serial.println("Starting setup...");
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  // pinMode(PLAYER1_BUTTON_W, INPUT_PULLUP);
  // pinMode(PLAYER1_BUTTON_R, INPUT_PULLUP);
  // pinMode(PLAYER1_BUTTON_B, INPUT_PULLUP);
  // pinMode(PLAYER2_BUTTON_W, INPUT_PULLUP);
  // pinMode(PLAYER2_BUTTON_R, INPUT_PULLUP);
  // pinMode(PLAYER2_BUTTON_B, INPUT_PULLUP);
  pinMode(START_BUTTON, INPUT_PULLUP);

  delay(2000);
  Serial.println("");
  gameManager = GameManager();
  gameManager.settings.buzzerVolume = 20; // in percentage
  gameManager.settings.maxTimeout = 3000;
  Serial.println("Game Manager Initialized");
}

int playerButtonRead()
{
  if (digitalRead(PLAYER1_BUTTON_R) == LOW)
  {
    Serial.println("RED BUTTON PRESSED");
  }
  
  return (digitalRead(PLAYER1_BUTTON_R) == LOW) ? 0x01 : 0;
  // int player1Button = (digitalRead(PLAYER1_BUTTON_W)||digitalRead(PLAYER1_BUTTON_B) || digitalRead(PLAYER1_BUTTON_R)) ? 0 : 0x10;
  // if (digitalRead(PLAYER1_BUTTON_W) == LOW || digitalRead(PLAYER2_BUTTON_W) == LOW)
  // {
  //   return 0x01;
  // }
  // if (digitalRead(PLAYER1_BUTTON_R) == LOW || digitalRead(PLAYER2_BUTTON_R) == LOW)
  // {
  //   return 0x02;
  // }
  // if (digitalRead(PLAYER1_BUTTON_B) == LOW || digitalRead(PLAYER2_BUTTON_B) == LOW)
  // {
  //   return 0x03;
  // }
  // return 0;
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
  else if (digitalRead(START_BUTTON) == LOW && gameManager.running == 1)
  {
    Serial.println("Button pressed");
    gameManager.endGame();
    delay(100); // Debounce delay
  }

  int playerButton = playerButtonRead();
  if (playerButton != 0 && gameManager.running == 1)
  {
    int player = (playerButton & 0x0F);
    int color = (playerButton & 0xF0) >> 4;
    lastButtonsPressed[(player-1) * 3 + color] = millis();
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

  if (gameManager.running == 1 && coolDownTime > 0)
  {
    if (millis() - coolDownTime > coolDown)
    {
      coolDownTime = 0;
    }
  }
  else if (gameManager.running == 1)

  {
    if (!gameManager.ledState && waitToTurnOn == 0)
    {
      delayTime = random(50, gameManager.settings.maxTimeout);
      Serial.print("Random delay: ");
      Serial.println(delayTime);
      gameManager.setColor(gameManager.getRandomColorIndex());
      waitToTurnOn = 1;
    }
    else if (waitToTurnOn == 1 && millis() - lastBlink > (delayTime+500))
    {
      gameManager.turnOnLED(gameManager.currentColor);
      analogWrite(BUZZER, gameManager.settings.buzzerVolume * 2.55); // Set buzzer volume
      delay(50);
      analogWrite(BUZZER, 0);                                 // Turn off buzzer
      lastBlink = millis();
      waitToTurnOn = 0;
    }
    else
    {
      if (millis() - lastBlink > 500)
      {
        gameManager.turnOffLED();
        coolDown = 500;
        coolDownTime = millis();
      }
    }
  }

  delay(100);
}
