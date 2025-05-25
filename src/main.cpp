#include <Arduino.h>
#include <gameManager.hpp>
#include <time.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <ESP32Servo.h>

GameManager gameManager;
time_t lastBlink;
int waitToTurnOn = 0;
int delayTime = 0;
int coolDown = 0;
int hasBeenPressed = 0;
int pos = 0;
int servoPositionTarget = 180;
int firstLoop = 1;
time_t coolDownTime;
time_t buzzTime = 0;
time_t lastButtonsPressed[6] = {0, 0, 0, 0, 0, 0};
Adafruit_NeoPixel WS2812B(STRIP_COUNT, STRIP_WS2812B, NEO_GRB + NEO_KHZ800);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Servo motor;

void setup()
{
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  Serial.begin(115200);

  Serial.println("Starting setup...");
  motor.attach(SERVO_PAS_LE_TRUC_KON_A_DAN_LE_TETE, 500, 2400);
  WS2812B.begin();
  WS2812B.setBrightness(255);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(POTENTIOMETER, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(PLAYER1_BUTTON_R, INPUT_PULLUP);
  // pinMode(PLAYER1_BUTTON_G, INPUT_PULLUP);
  // pinMode(PLAYER1_BUTTON_B, INPUT_PULLUP);
  // pinMode(PLAYER2_BUTTON_R, INPUT_PULLUP);
  // pinMode(PLAYER2_BUTTON_G, INPUT_PULLUP);
  // pinMode(PLAYER2_BUTTON_B, INPUT_PULLUP);
  pinMode(START_BUTTON, INPUT_PULLUP);

  Wire.begin(OLED_DISPLAY_SDA, OLED_DISPLAY_SCL); // SDA, SCL
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  delay(100);
  display.clearDisplay();
  display.display();
  delay(200);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor((SCREEN_WIDTH / 2) - ((12 * 6) / 2), 0);
  display.println("Reflex");
  display.setCursor((SCREEN_WIDTH / 2) - ((12 * 7) / 2), 20);
  display.println("Fighter");
  display.setCursor((SCREEN_WIDTH / 2) - (15 * 6) / 2, 50);
  display.setTextSize(1);
  display.println("> Press START <");
  display.display();

  delay(2000);
  Serial.println("");
  gameManager = GameManager();
  gameManager.settings.buzzerVolume = 20; // in percentage
  gameManager.settings.maxScore = 10;     // Set max score for the game
  gameManager.settings.maxTimeout = 3000;
  Serial.println("Game Manager Initialized");
}

int playerButtonRead() // TO FINISH
{
  if (digitalRead(PLAYER1_BUTTON_R) == LOW && lastButtonsPressed[0] + 1000 < millis()
      // || digitalRead(PLAYER2_BUTTON_R) == LOW
  )
  {
    return (digitalRead(PLAYER1_BUTTON_R) == LOW) ? 0x01 : 0x02; // Player 1 or 2, Red
  }
  // else if (digitalRead(PLAYER1_BUTTON_G) == LOW || digitalRead(PLAYER2_BUTTON_G) == LOW)
  // {
  //   return (digitalRead(PLAYER1_BUTTON_G) == LOW) ? 0x11 : 0x12; // Player 1 or 2, Green
  // }
  // else if (digitalRead(PLAYER1_BUTTON_B) == LOW || digitalRead(PLAYER2_BUTTON_B) == LOW)
  // {
  //   return (digitalRead(PLAYER1_BUTTON_B) == LOW) ? 0x21 : 0x22; // Player 1 or 2, Blue
  // }
  return 0; // No button pressed
}

int buzz()
{
  analogWrite(BUZZER, gameManager.settings.buzzerVolume * 2.55); // Set buzzer volume
  buzzTime = millis();
  return 0;
}

int lastPotentiometerValue = 0;

void loop()
{
  if (firstLoop)
  {
    motor.write(90);
    firstLoop = 0;
  }
  // Serial.println(digitalRead(START_BUTTON));
  // WS2812B.setPixelColor(0, WS2812B.Color(255, 0, 0));

  int potentiometerValue = analogRead(POTENTIOMETER);

  if (potentiometerValue > lastPotentiometerValue + 150 || potentiometerValue < lastPotentiometerValue - 150)
  {
    Serial.println("Potentiometer value changed: ");
    Serial.println(potentiometerValue);
    // buzzer volume
    gameManager.settings.buzzerVolume = map(potentiometerValue, 0, 4095, 0, 100);
    analogWrite(BUZZER, gameManager.settings.buzzerVolume * 2.55); // Set buzzer volume
    delay(100);                                                    // Debounce delay
    analogWrite(BUZZER, 0);                                        // Turn off buzzer
    lastPotentiometerValue = potentiometerValue;
    delay(100); // Debounce delay
  }

  uint32_t p1color = WS2812B.Color(255, 0, 0);
  uint32_t p2color = WS2812B.Color(0, 0, 255);
  // Divide the strip into two halves proportional to the players scores
  int maxScore = gameManager.player1.score + gameManager.player2.score;
  int p1Count = (gameManager.player1.score * STRIP_COUNT) / maxScore;
  int p2Count = STRIP_COUNT - p1Count;

  if (maxScore == 0)
  {
    p1Count = STRIP_COUNT / 2; // Default to half if no scores
    p2Count = STRIP_COUNT / 2;
  }

  for (int i = 0; i < STRIP_COUNT; i++)
  {
    if (i < p1Count)
    {
      WS2812B.setPixelColor(i, p1color);
    }
    else
    {
      WS2812B.setPixelColor(i, p2color);
    }
  }

  WS2812B.show();

  if (digitalRead(START_BUTTON) == LOW && gameManager.running == 0)
  {
    Serial.println("Button pressed");
    gameManager.startGame();
    display.clearDisplay();
    display.display();
    delay(100); // Debounce delay
  }
  else if (digitalRead(START_BUTTON) == LOW && gameManager.running == 1)
  {
    Serial.println("Button pressed");
    gameManager.endGame(display, motor, WS2812B);
    delay(100); // Debounce delay
  }

  int playerButton = playerButtonRead();
  if (playerButton != 0 && gameManager.running == 1 && hasBeenPressed == 0)
  {
    int player = (playerButton & 0x0F); // Extract player number (1 or 2)
    int color = (playerButton & 0xF0) >> 4;
    lastButtonsPressed[(player - 1) * 3 + color] = millis();
    Serial.print("Player ");
    Serial.print(player);
    Serial.print(" pressed button ");
    Serial.println(color);
    hasBeenPressed = 1;
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
      buzz();
      if (player == 1)
      {
        gameManager.player2.increaseScore(1);
      }
      else
      {
        gameManager.player1.increaseScore(1);
      }
    }
    gameManager.player1.displayScore();
    gameManager.player2.displayScore();
  }

  if (gameManager.running == 1)
  {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.print("Player 1");
    display.setCursor((SCREEN_WIDTH - ((8 * 6))), 0);
    display.print("Player 2");
    display.setTextSize(1);
    display.setCursor(0, 20);
    display.print(gameManager.player1.score);
    display.setCursor((SCREEN_WIDTH - 3 * 6), 20);
    display.print(gameManager.player2.score);
    display.setCursor(0, 40);
    display.display();
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
      hasBeenPressed = 0;
    }
    else if (waitToTurnOn == 1 && millis() - lastBlink > (delayTime + 500))
    {
      gameManager.turnOnLED(gameManager.currentColor);

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

  if (buzzTime > 0 && millis() - buzzTime > 200)
  {
    analogWrite(BUZZER, 0); // Turn off buzzer after 1 second
    buzzTime = 0;
  }

  if (gameManager.player1.score >= gameManager.settings.maxScore || gameManager.player2.score >= gameManager.settings.maxScore)
  {
    gameManager.endGame(display, motor, WS2812B);
    return;
  }

  delay(10);
}
