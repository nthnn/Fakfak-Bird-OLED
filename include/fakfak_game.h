#ifndef FAKFAK_GAME_H
#define FAKFAK_GAME_H

#include <Arduino.h>

class FakfakGame {
public:
    static void gameSetup();
    static void gameLoop();

private:
    static void screenWipe(int speed);
    static void textAtCenter(int y, String txt);
};

#endif