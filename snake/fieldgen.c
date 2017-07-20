#include <math.h>
#include <stdbool.h>
#include "fieldgen.h"
#include "constants.h"
#include "common.h"

cell field[FIELD_SIZE][FIELD_SIZE];
extern player snake;
extern bool gameover;

void initialize_field() {
    int i, j;

    for (i = 0; i < FIELD_SIZE; i++)
        for (j = 0; j < FIELD_SIZE; j++) {
            field[i][j].time_to_live = 0;
            field[i][j].has_apple = false;
        }

    field[START_ROW][START_COLUMN].time_to_live = START_LENGTH;
    field[START_ROW + 1][START_COLUMN].time_to_live = START_LENGTH - 1;
}

void create_apple() {
    int i, j;
    int free_cells_high = 0;
    int apple_cell;
    position free_cells[FIELD_SIZE * FIELD_SIZE];

    for (i = 0; i < FIELD_SIZE; i++) {
        free_cells[i].row = NOT_EMPTY;
        free_cells[j].column = NOT_EMPTY;
    }

    for (i = 0; i < FIELD_SIZE; i++)
        for (j = 0; j < FIELD_SIZE; j++)
            if (field[i][j].time_to_live == 0) {
                free_cells[free_cells_high].row = i;
                free_cells[free_cells_high++].column = j;
            }

    apple_cell = rand() % free_cells_high;
    i = free_cells[apple_cell].row;
    j = free_cells[apple_cell].column;

    field[i][j].has_apple = true;
}

void refresh_field() {
    int i, j;

    for (i = 0; i < FIELD_SIZE; i++)
        for (j = 0; j < FIELD_SIZE; j++)
            if (field[i][j].time_to_live != 0)
                field[i][j].time_to_live--;
}

void move_snake() {
    switch (snake.movement) {
        case GO_UP:
            snake.head.row--;
            break;
        case GO_DOWN:
            snake.head.row++;
            break;
        case GO_LEFT:
            snake.head.column--;
            break;
        case GO_RIGHT:
            snake.head.column++;
            break;
    }
    if (snake.head.row >= FIELD_SIZE || snake.head.row < 0 || snake.head.column >= FIELD_SIZE || snake.head.column < 0
        || field[snake.head.row][snake.head.column].time_to_live != 0)
        gameover = true;
    else {
        if (field[snake.head.row][snake.head.column].has_apple) {
            eat_apple();
            create_apple();
        }
        field[snake.head.row][snake.head.column].time_to_live = snake.length;
    }
}

void eat_apple() {
    int i, j;

    snake.length++;
    for (i = 0; i < FIELD_SIZE; i++)
        for (j = 0; j < FIELD_SIZE; j++) {
            if (field[i][j].time_to_live != 0)
                field[i][j].time_to_live++;
        }

    tone(TONE_PIN, snake.sound_frequency, TONE_DURATION);
    snake.sound_frequency += TONE_GAP;

    field[snake.head.row][snake.head.column].has_apple = false;
}
