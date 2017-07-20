#include <stdlib.h>
#include <stdbool.h>
#include "LedControl.h"
#include "interface.h"
#include "constants.h"
#include "common.h"

extern player snake;
extern cell field[FIELD_SIZE][FIELD_SIZE];
extern bool gamestarted, gameover, continue_flag;
LedControl lc = LedControl(12, 11, 10, 1);

void setup_led() {
    lc.shutdown(0, false);
    lc.setIntensity(0, LED_INTENSITY_MEDIUM);
    lc.clearDisplay(0);
}

void show_field() {
    int i, j;

    for (i = 0; i < FIELD_SIZE; i++)
        for (j = 0; j < FIELD_SIZE; j++) {
            if (field[i][j].time_to_live != 0 || field[i][j].has_apple)
                lc.setLed(0, i, j, true);
            else
                lc.setLed(0, i, j, false);
        }
}

void startgame_sequence() {
    byte str_startgame[STR_STARTGAME_LENGTH] = {
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

    output_ledstr(str_startgame, STR_STARTGAME_LENGTH);
    if (continue_flag) {
        continue_flag = false;
        gamestarted = true;
    }
    lc.clearDisplay(0);
}

void gameover_sequence() {
    byte str_gameover[STR_GAMEOVER_LENGTH] = {
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
    int i = 0, j = 0;
    int soundfreq = TONE_MINLEVEL;

    delay(DELAY_GAMEOVER);
    lc.clearDisplay(0);
    while (j < FIELD_SIZE) {
        lc.setLed(0, i, j, true);
        tone(TONE_PIN, soundfreq, TONE_DURATION);
        soundfreq += TONE_GAP;
        delay(TONE_DURATION + TONE_CORRECTION);
        if (i == FIELD_SIZE - 1) {
            i = 0;
            j++;
        } else {
            i++;
        }
    }
    lc.clearDisplay(0);

    output_ledstr(str_gameover, STR_GAMEOVER_LENGTH);
    gameover = false;
    if (continue_flag)
        continue_flag = false;
}

void output_ledstr(byte str_output[], int strsize) {
    int i, j;
    byte screen[FIELD_SIZE];
    for (i = 0; i < FIELD_SIZE; i++)
        screen[i] = B00000000;
    show_screen(screen);

    for (i = 0; i < strsize; i++) {
        for (j = 0; j < (FIELD_SIZE - 1); j++)
            screen[j] = screen[j + 1];
        screen[j] = str_output[i];
        show_screen(screen);
    }

    for (i = 0; i < FIELD_SIZE; i++) {
        for (j = 0; j < (FIELD_SIZE - 1); j++)
            screen[j] = screen[j + 1];
        screen[j] = B00000000;
        show_screen(screen);
    }
}

void show_screen(byte screen[FIELD_SIZE]) {
    int i;

    if (!continue_flag) {
        for (i = 0; i < FIELD_SIZE; i++)
            lc.setColumn(0, i, screen[i]);
        get_skipcond();
        delay(DELAY_LETTER);
    }
}

void get_direction() {
    int x_input = analogRead(CONTROL_PORT_X) + CONTROL_XCORRECTION;
    int y_input = - (analogRead(CONTROL_PORT_Y) + CONTROL_YCORRECTION);

    if ((x_input * x_input + y_input * y_input) > CONTROL_NEUTRALZONE * CONTROL_NEUTRALZONE) {
        if (y_input > abs(x_input)) {
            if (snake.movement != GO_DOWN)
                snake.movement = GO_UP;
        } else if (y_input < -abs(x_input)) {
            if (snake.movement != GO_UP)
                snake.movement = GO_DOWN;
        } else if (x_input < -CONTROL_NEUTRALZONE) {
            if (snake.movement != GO_RIGHT)
                snake.movement = GO_LEFT;
        } else if (x_input > CONTROL_NEUTRALZONE) {
            if (snake.movement != GO_LEFT)
                snake.movement = GO_RIGHT;
        }
    }
}

void get_skipcond() {
    int x_input = analogRead(CONTROL_PORT_X);
    int y_input = analogRead(CONTROL_PORT_Y);

    if (x_input > LIMIT_X_HIGH || x_input < LIMIT_X_LOW || y_input > LIMIT_Y_HIGH || y_input < LIMIT_Y_LOW)
        continue_flag = true;
}
