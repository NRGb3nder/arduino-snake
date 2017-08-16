#include "Arduino.h"
#include "snake.hpp"

const int TONE_GAP = 100;
const int TONE_PIN = 4;
const int TONE_DURATION = 100;
const int TONE_MINLEVEL = 3000;

extern Field playingField;
extern bool isGameover;

Snake::Snake()
{
    fLength = START_LENGTH;
    fMovement = MOV_UP;
    fHead.row = START_ROW;
    fHead.column = START_COLUMN;
    fSoundFrequency = TONE_MINLEVEL;
}

void Snake::move()
{
    switch (fMovement) {
        case MOV_UP:
            fHead.row--;
            break;
        case MOV_DOWN:
            fHead.row++;
            break;
        case MOV_LEFT:
            fHead.column--;
            break;
        case MOV_RIGHT:
            fHead.column++;
            break;
    }
    if (fHead.row >= FIELD_SIZE || fHead.row < 0 || fHead.column >= FIELD_SIZE || fHead.column < 0
    || playingField.getCellLifetime(fHead.row, fHead.column) != 0) {
        isGameover = true;
    } else {
        if (playingField.getCellAppleInfo(fHead.row, fHead.column)) {
            eatApple();
        }
        playingField.setCellLifetime(fHead.row, fHead.column, fLength);
    }
}

void Snake::eatApple()
{
    fLength++;
    playingField.delayCellsLifetimeCount();
    playsoundAppleFound();
    fSoundFrequency += TONE_GAP;
    playingField.setCellAppleInfo(fHead.row, fHead.column, false);
    playingField.createApple();
}

void Snake::playsoundAppleFound() const
{
    tone(TONE_PIN, fSoundFrequency, TONE_DURATION);
}

Direction Snake::getMovementDirection() const
{
    return fMovement;
}

void Snake::setMovementDirection(Direction aMovementDirection)
{
    fMovement = aMovementDirection;
}

int Snake::getLength() const
{
    return fLength;
}

