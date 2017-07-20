#include <stdbool.h>
#include "constants.h"
#include "common.h"
#include "fieldgen.h"
#include "interface.h"

extern cell field[FIELD_SIZE][FIELD_SIZE];
player snake;
bool gameover, gamestarted, continue_flag;

void setup() {
    gameover = false;
    gamestarted = false;
    continue_flag = false;
    setup_led();
    initialize_field();
    snake = {START_LENGTH, GO_UP, {START_ROW, START_COLUMN}, TONE_MINLEVEL};
    create_apple();
}

void loop() {
    if (gamestarted)
        setup;
    else
        while (!gamestarted)
            startgame_sequence();
    while (!gameover) {
        get_direction();
        refresh_field();
        move_snake();
        if (!gameover) {
            show_field();
            delay(DELAY_GENERAL);
        }
    }
    gameover_sequence();
}

void playsound_apple() {
  tone(TONE_PIN, snake.sound_frequency, TONE_DURATION);
}

