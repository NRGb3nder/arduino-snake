#include "stdbool.h"
#include "time.h"
#include "stdlib.h"
#include "math.h"
#include "LedControl.h"

#define     FIELD_SIZE            8
#define     MEDIUM_LED_INTENSITY  8
#define     START_LENGTH          2
#define     START_ROW             4
#define     START_COLUMN          4
#define     DELAY_TIME            400
#define     X_PORT                1
#define     Y_PORT                0
#define     SENSIVITY_LEFT        700
#define     SENSIVITY_RIGHT       300
#define     SENSIVITY_DOWN        600
#define     SENSIVITY_UP          400
#define     NOT_EMPTY             -1

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
} player;

LedControl lc = LedControl(12, 11, 10, 1);
cell field[FIELD_SIZE][FIELD_SIZE];
player snake = {START_LENGTH, GO_UP, {START_ROW, START_COLUMN}};
bool gameover = false;

void initialize_field();
void create_apple();
void get_direction();
void refresh_field();
void move_snake();
void eat_apple();
void show_field();

void setup() {
    lc.shutdown(0, false);
    lc.setIntensity(0, MEDIUM_LED_INTENSITY);
    lc.clearDisplay(0);
    initialize_field();
    srand(time(NULL));  // setting seed for apple generation
    create_apple();
}

void loop() {
    while (!gameover) {
        get_direction();
        refresh_field();
        move_snake();
        if (!gameover) {
            show_field();
            delay(DELAY_TIME);
        }
    }
}

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

void get_direction() {
    int x_input = analogRead(X_PORT);
    int y_input = analogRead(Y_PORT);
    
    if (x_input > SENSIVITY_LEFT && snake.movement != GO_RIGHT)
        snake.movement = GO_LEFT;
    else if (x_input < SENSIVITY_RIGHT && snake.movement != GO_LEFT)
        snake.movement = GO_RIGHT;
    else if (y_input > SENSIVITY_DOWN && snake.movement != GO_UP)
        snake.movement = GO_DOWN;
    else if (y_input < SENSIVITY_UP && snake.movement != GO_DOWN)
        snake.movement = GO_UP;
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
    field[snake.head.row][snake.head.column].has_apple = false;
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
