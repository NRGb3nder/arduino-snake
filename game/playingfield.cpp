#include "Arduino.h"
#include "playingfield.hpp"
#include "snake.hpp"

const int PIN_UNCONNECTED = 9; // any pin that will always be unconnected
const int NOT_EMPTY = -1;
const int RESULT_VICTORY = 1;

Field::Field()
{
    randomSeed(analogRead(PIN_UNCONNECTED));

    int i;
    int j;

    for (i = 0; i < FIELD_SIZE; i++) {
        for (j = 0; j < FIELD_SIZE; j++) {
            fCellField[i][j].lifetime = 0;
            fCellField[i][j].hasApple = false;
        }
    }

    fCellField[START_ROW][START_COLUMN].lifetime = START_LENGTH;
    fCellField[START_ROW + 1][START_COLUMN].lifetime = START_LENGTH - 1;
}

int Field::createApple()
{
    extern Snake player;
    int i;
    int j;
    int freeCellsQuantity = 0;
    int appleCell;
    Position *freeCells;
    freeCells = new Position [FIELD_SIZE * FIELD_SIZE - player.getLength()];
    
    for (i = 0; i < FIELD_SIZE; i++) {
        for (j = 0; j < FIELD_SIZE; j++) {
            if (fCellField[i][j].lifetime == 0) {
                freeCells[freeCellsQuantity].row = i;
                freeCells[freeCellsQuantity++].column = j;
            }
        }
    }
    if (freeCellsQuantity == 0) {
        delete[] freeCells;
        return RESULT_VICTORY;
    } else {
        appleCell = random(0, freeCellsQuantity);
        i = freeCells[appleCell].row;
        j = freeCells[appleCell].column;
        fCellField[i][j].hasApple = true;
        delete[] freeCells;
        return 0;
    }
}

void Field::update()
{
    int i;
    int j;

    for (i = 0; i < FIELD_SIZE; i++) {
        for (j = 0; j < FIELD_SIZE; j++) {
            if (fCellField[i][j].lifetime != 0) {
                fCellField[i][j].lifetime--;
            }
        }
    }
}

void Field::delayCellsLifetimeCount()
{
    int i;
    int j;

    for (i = 0; i < FIELD_SIZE; i++) {
        for (j = 0; j < FIELD_SIZE; j++) {
            if (fCellField[i][j].lifetime != 0) {
                fCellField[i][j].lifetime++;
            }
        }
    }
}

int Field::getCellLifetime(int aRow, int aColumn) const
{
    return fCellField[aRow][aColumn].lifetime;
}

void Field::setCellLifetime(int aRow, int aColumn, int aValue)
{
    fCellField[aRow][aColumn].lifetime = aValue;
}

bool Field::getCellAppleInfo(int aRow, int aColumn) const
{
    return fCellField[aRow][aColumn].hasApple;
}

void Field::setCellAppleInfo(int aRow, int aColumn, bool aFieldValue)
{
    fCellField[aRow][aColumn].hasApple = aFieldValue;
}
