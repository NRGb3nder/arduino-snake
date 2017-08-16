#include "Arduino.h"
#include "interface.hpp"
#include "common.hpp"
#include "playingfield.hpp"
#include "snake.hpp"
#include "LedControl.h"

const int PIN_DATAIN = 12;
const int PIN_CLK = 11;
const int PIN_LOAD = 10;
const int DRIVER_QUANTITY = 1;
const int DEVICE_ADDRESS_LED = 0;
const int LED_INTENSITY_MEDIUM = 8;
const int CONTROL_PORT_X = 1;
const int CONTROL_PORT_Y = 0;
const int CONTROL_CORRECTION_X = -507;
const int CONTROL_CORRECTION_Y = -507;
const int CONTROL_NEUTRALZONE = 250;
const int LIMIT_X_HIGH = 600;
const int LIMIT_X_LOW = 400;
const int LIMIT_Y_HIGH = 600;
const int LIMIT_Y_LOW = 400;
const int DELAY_LETTER = 100;
const int DELAY_GAMEOVER = 800;
const int STR_STARTGAME_LENGTH = 43;
const int STR_GAMEOVER_LENGTH = 80;
const uint8_t STR_STARTGAME[STR_STARTGAME_LENGTH] = {
    B00000000, B11111111, B10010000, B01100000,                         // 'P'
    B00000000, B11111111, B10010000, B01101111,                         // 'R'
    B00000000, B11111111, B10010001, B10010001,                         // 'E'
    B00000000, B01100010, B10010001, B01001110,                         // 'S'
    B00000000, B01100010, B10010001, B01001110,                         // 'S'
    B00000000, B00000000, B00000000,                                    // ' '
    B00000000, B01100010, B10010001, B01001110,                         // 'S'
    B00000000, B10000000, B11111111, B10000000,                         // 'T'
    B00000000, B01111111, B10001000, B01111111,                         // 'A'
    B00000000, B11111111, B10010000, B01101111,                         // 'R'
    B00000000, B10000000, B11111111, B10000000,                         // 'T'
};
const uint8_t STR_GAMEOVER[STR_GAMEOVER_LENGTH] = {
    B00000000, B01111110, B10001001, B10001111,                         // 'G'
    B00000000, B01111111, B10001000, B01111111,                         // 'A'
    B00000000, B11111111, B01000000, B00100000, B01000000, B11111111,   // 'M'
    B00000000, B11111111, B10010001, B10010001,                         // 'E'
    B00000000, B00000000, B00000000,                                    // ' '
    B00000000, B01111110, B10000001, B10000001, B01111110,              // 'O'
    B00000000, B11111110, B00000001, B11111110,                         // 'V'
    B00000000, B11111111, B10010001, B10010001,                         // 'E'
    B00000000, B11111111, B10010000, B01101111,                         // 'R'
    B00000000, B11111101,                                               // '!'
    B00000000, B00000000, B00000000,                                    // ' '
    B00000000, B10000000, B11111111, B10000000,                         // 'T'
    B00000000, B11111111, B10010000, B01101111,                         // 'R'
    B00000000, B11100000, B00011111, B11100000,                         // 'Y'
    B00000000, B00000000, B00000000,                                    // ' '
    B00000000, B01111111, B10001000, B01111111,                         // 'A'
    B00000000, B01111110, B10001001, B10001111,                         // 'G'
    B00000000, B01111111, B10001000, B01111111,                         // 'A'
    B00000000, B10000001, B11111111, B10000001,                         // 'I'
    B00000000, B11111111, B00100000, B00010000, B00001000, B11111111    // 'N'
};

LedControl lc = LedControl(PIN_DATAIN, PIN_CLK, PIN_LOAD, DRIVER_QUANTITY);
extern Field playingField;
extern Snake player;
extern bool continuationCondition;

void setupMatrix()
{
    lc.shutdown(DEVICE_ADDRESS_LED, false);
    lc.setIntensity(DEVICE_ADDRESS_LED, LED_INTENSITY_MEDIUM);
    lc.clearDisplay(DEVICE_ADDRESS_LED);
}

void showField()
{
    int i;
    int j;

    for (i = 0; i < FIELD_SIZE; i++) {
        for (j = 0; j < FIELD_SIZE; j++) {
            if (playingField.getCellLifetime(i, j) != 0 || playingField.getCellAppleInfo(i, j)) {
                lc.setLed(DEVICE_ADDRESS_LED, i, j, true);
            } else {
                lc.setLed(DEVICE_ADDRESS_LED, i, j, false);
            }
        }
    }
}

void ctrlSetDirection()
{
    int inputX = -(analogRead(CONTROL_PORT_X) + CONTROL_CORRECTION_X);
    int inputY = -(analogRead(CONTROL_PORT_Y) + CONTROL_CORRECTION_Y);

    if ((inputX * inputX + inputY * inputY) > (CONTROL_NEUTRALZONE * CONTROL_NEUTRALZONE)) {
        if (inputY > abs(inputX)) {
            if (player.getMovementDirection() != MOV_DOWN) {
                player.setMovementDirection(MOV_UP);
            }
        } else if (inputY < -abs(inputX)) {
            if (player.getMovementDirection() != MOV_UP) {
                player.setMovementDirection(MOV_DOWN);
            }
        } else if (inputX < -CONTROL_NEUTRALZONE) {
            if (player.getMovementDirection() != MOV_RIGHT) {
                player.setMovementDirection(MOV_LEFT);
            }
        } else if (inputX > CONTROL_NEUTRALZONE) {
            if (player.getMovementDirection() != MOV_LEFT) {
                player.setMovementDirection(MOV_RIGHT);
            }
        }
    }
}

bool ctrlGetContinuationCondition()
{
    int inputX = analogRead(CONTROL_PORT_X);
    int inputY = analogRead(CONTROL_PORT_Y);

    return inputX > LIMIT_X_HIGH || inputX < LIMIT_X_LOW || inputY > LIMIT_Y_HIGH || inputY < LIMIT_Y_LOW;
}

void launchStartgameSequence()
{
    outputLedStr(STR_STARTGAME, STR_STARTGAME_LENGTH);
    lc.clearDisplay(DEVICE_ADDRESS_LED);
}

void launchGameoverSequence()
{
    delay(DELAY_GAMEOVER);
    outputLedStr(STR_GAMEOVER, STR_GAMEOVER_LENGTH);
    reboot();
}

void outputLedStr(const uint8_t aStrOutput[], int aStrsize)
{
    int i;
    int j;
    uint8_t ledSegment[FIELD_SIZE];

    for (i = 0; i < FIELD_SIZE; i++) {
        ledSegment[i] = B00000000;
    }
    showLedSegment(ledSegment);

    for (i = 0; i < aStrsize; i++) {
        for (j = 0; j < (FIELD_SIZE - 1); j++) {
            ledSegment[j] = ledSegment[j + 1];
        }
        ledSegment[j] = aStrOutput[i];
        showLedSegment(ledSegment);
    }

    for (i = 0; i < FIELD_SIZE; i++) {
        for (j = 0; j < (FIELD_SIZE - 1); j++) {
            ledSegment[j] = ledSegment[j + 1];
        }
        ledSegment[j] = B00000000;
        showLedSegment(ledSegment);
    }
}

void showLedSegment(const uint8_t aLedSegment[])
{
    if (!continuationCondition) {
        for (int i = 0; i < FIELD_SIZE; i++) {
            lc.setColumn(DEVICE_ADDRESS_LED, i, aLedSegment[i]);
        }
        continuationCondition = ctrlGetContinuationCondition();
        delay(DELAY_LETTER);
    }
}

void reboot()
{
    asm volatile ("jmp 0x00");
}
