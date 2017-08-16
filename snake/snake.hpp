#pragma once

#include "common.hpp"
#include "playingfield.hpp"

enum Direction { MOV_UP, MOV_DOWN, MOV_LEFT, MOV_RIGHT };

class Snake
{
private:
    int fLength;
    Direction fMovement;
    Position fHead;
    int fSoundFrequency;
public:
    Snake();
    void move();
    void eatApple();
    void playsoundAppleFound() const;
    Direction getMovementDirection() const;
    void setMovementDirection(Direction);
    int getLength() const;
};
