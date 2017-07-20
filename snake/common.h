#pragma once

#include <stdbool.h>
#include "constants.h"

typedef struct {
    int time_to_live;
    bool has_apple;
} cell;

enum direction {GO_UP, GO_DOWN, GO_LEFT, GO_RIGHT};

typedef struct {
    int row;
    int column;
} position;

typedef struct {
    int length;
    enum direction movement;
    position head;
    int sound_frequency;
} player;

void playsound_apple();
