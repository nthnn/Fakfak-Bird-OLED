#include <Arduino.h>
#include <fakfak_game.h>

void setup() {
  FakfakGame::gameSetup();
}

void loop() {
  FakfakGame::gameLoop();
}