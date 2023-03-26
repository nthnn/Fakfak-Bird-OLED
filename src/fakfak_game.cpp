#include <SPI.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

#include <fakfak_bird.h>
#include <fakfak_defs.h>
#include <fakfak_game.h>

Adafruit_SSD1306 gameDisplay(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT, &Wire, -1);

int wallX[2], wallY[2];
int wallGap = 35, wallWidth = 10;

int gameState = 1, score = 0, highScore = 0;
int birdX = (int) gameDisplay.width() / 4, birdY, momentum = 0;

void FakfakGame::screenWipe(int speed) {
  for(int i = 0; i < gameDisplay.height(); i += speed) {
    gameDisplay.fillRect(0, i, gameDisplay.width(), speed, WHITE);
    gameDisplay.display();
  }

  for(int i = 0; i < gameDisplay.height(); i += speed) {
    gameDisplay.fillRect(0, i, gameDisplay.width(), speed, BLACK);
    gameDisplay.display();
  }
}

void FakfakGame::textAtCenter(int y, String txt) {
  gameDisplay.setCursor(gameDisplay.width() / 2 - txt.length() * 4 - 3, y);
  gameDisplay.print(txt);
}

void FakfakGame::gameSetup() {
  if(!gameDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    while(true);

  gameDisplay.setTextColor(WHITE);
  gameDisplay.clearDisplay();

  pinMode(GAME_FLAP_BUTTON, INPUT_PULLUP);
  randomSeed(analogRead(0));
}

void FakfakGame::gameLoop() {
  if(gameState == GAME_STATE_IN_GAME) {
    gameDisplay.clearDisplay();

    if(digitalRead(GAME_FLAP_BUTTON) == HIGH)
      momentum = -4;

    momentum += 1;
    birdY += momentum;

    if(birdY < 0)
      birdY = 0;

    if(birdY > gameDisplay.height() - GAME_TILE_HEIGHT) {
      birdY = gameDisplay.height() - GAME_TILE_HEIGHT;
      momentum = -2;
    }

    if(momentum < 0) {
      if(random(2) == 0)
        gameDisplay.drawBitmap(birdX, birdY, wing_down_bmp, 16, 16, WHITE);
      else gameDisplay.drawBitmap(birdX, birdY, wing_up_bmp, 16, 16, WHITE);
    }
    else gameDisplay.drawBitmap(birdX, birdY, wing_up_bmp, 16, 16, WHITE);

    for(int i = 0 ; i < 2; i++) {
      gameDisplay.fillRect(wallX[i], 0, wallWidth, wallY[i], WHITE);
      gameDisplay.fillRect(wallX[i], wallY[i] + wallGap, wallWidth, gameDisplay.height() - wallY[i] + wallGap, WHITE);

      if(wallX[i] < 0) {
        wallY[i] = random(0, gameDisplay.height() - wallGap);
        wallX[i] = gameDisplay.width();
      }

      if(wallX[i] == birdX) {
        score++;
        highScore = max(score, highScore);
      }

      if((birdX + GAME_TILE_WIDTH > wallX[i] && birdX < wallX[i] + wallWidth) && (birdY < wallY[i] || birdY + GAME_TILE_HEIGHT > wallY[i] + wallGap)) {
        gameDisplay.display();
        delay(500);

        gameState = GAME_STATE_TEXT;
      }

      wallX[i] -= 4;
    }

    gameDisplay.setFont(&FreeSansBold9pt7b);
    textAtCenter(15, (String) score);
    gameDisplay.display();

    delay(GAME_SPEED);
  }
  else {
    screenWipe(10);

    gameDisplay.setFont(&FreeSansBold9pt7b);
    textAtCenter(16, "Fakfak Bird");
    gameDisplay.drawFastHLine(2, 19, GAME_SCREEN_WIDTH - 4, SSD1306_WHITE);

    gameDisplay.setFont(&FreeSans9pt7b);
    textAtCenter(gameDisplay.height() / 2 + 8, "Score: " + String(score));
    textAtCenter(gameDisplay.height() - 14 + 8, "Highest: " + String(highScore));

    gameDisplay.display();
    while(digitalRead(GAME_FLAP_BUTTON) != HIGH);

    birdY = gameDisplay.height() / 2;
    momentum = -4;

    wallX[0] = gameDisplay.width() ;
    wallY[0] = gameDisplay.height() / 2 - wallGap / 2;
    wallX[1] = gameDisplay.width() + gameDisplay.width() / 2;
    wallY[1] = gameDisplay.height() / 2 - wallGap / 1;

    score = 0;
    while (digitalRead(GAME_FLAP_BUTTON) == HIGH);

    screenWipe(10);
    gameState = GAME_STATE_IN_GAME;
  }
}