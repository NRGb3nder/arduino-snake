#pragma once

#include <stdint.h>

// LED output
void setupMatrix();
void showField();
void launchStartgameSequence();
void launchGameoverSequence();
void outputLedStr(const uint8_t [], int);
void showLedSegment(const uint8_t []);

// Controls
void ctrlSetDirection();
bool ctrlGetContinuationCondition();

// Arduino
void reboot();
