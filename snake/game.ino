#include "common.hpp"
#include "playingfield.hpp"
#include "snake.hpp"
#include "interface.hpp"

Field playingField;
Snake player;
bool isGameover = false;
bool continuationCondition = false;

void setup()
{
    setupMatrix();
    playingField.createApple();
}

void loop()
{
    while (!continuationCondition) {
        launchStartgameSequence();
    }
    continuationCondition = false;
    while (!isGameover) {
        ctrlSetDirection();
        playingField.update();
        player.move();
        if (!isGameover) {
            showField();
            delay(DELAY_GENERAL);
        }
    }
    launchGameoverSequence();
}
