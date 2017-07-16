#include "stdbool.h"
#include "stdlib.h"
#include "math.h"
#include "LedControl.h"

#define     FIELD_SIZE            8
#define     MEDIUM_LED_INTENSITY  8
#define     START_LENGTH          2
#define     START_ROW             4
#define     START_COLUMN          4
#define     DELAY_GENERAL         400
#define     DELAY_LETTER          200
#define     X_PORT                1
#define     Y_PORT                0
#define     SENSIVITY_LEFT        700
#define     SENSIVITY_RIGHT       300
#define     SENSIVITY_DOWN        600
#define     SENSIVITY_UP          400
#define     NOT_EMPTY             -1
#define     STR_GAMEOVER_LENGTH   40
#define     STR_STARTGAME_LENGTH  43
#define     LIMIT_X_HIGH          600
#define     LIMIT_X_LOW           400
#define     LIMIT_Y_HIGH          600
#define     LIMIT_Y_LOW           400

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
bool gameover = false, gamestarted = false, skip_flag = false;

void initialize_field();
void create_apple();
void get_direction();
void refresh_field();
void move_snake();
void eat_apple();
void show_field();
void startgame_sequence();
void gameover_sequence();
void output_ledstr(byte str_output[], int strsize);
void show_screen(byte screen[FIELD_SIZE]);
void get_skipcond();


void setup() {
    lc.shutdown(0, false);
    lc.setIntensity(0, MEDIUM_LED_INTENSITY);
    lc.clearDisplay(0);
    initialize_field();
    create_apple();
}

void loop() {
    while (!gamestarted)
        startgame_sequence();
    lc.clearDisplay(0);
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
            B00000000, B11111101                                                // '!'
    };

    output_ledstr(str_gameover, STR_GAMEOVER_LENGTH);
    gamestarted = false;
    gameover = false;
    if (skip_flag)
        skip_flag = false;
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
    if (skip_flag) {
        skip_flag = false;
        gamestarted = true;
    }
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

    if (!skip_flag) {
        for (i = 0; i < FIELD_SIZE; i++)
            lc.setColumn(0, i, screen[i]);
        get_skipcond();
        delay(DELAY_LETTER);
    }
}

void get_skipcond() {
    int x_input = analogRead(X_PORT);
    int y_input = analogRead(Y_PORT);

    if (x_input > LIMIT_X_HIGH || x_input < LIMIT_X_LOW || y_input > LIMIT_Y_HIGH || y_input < LIMIT_Y_LOW)
        skip_flag = true;
}