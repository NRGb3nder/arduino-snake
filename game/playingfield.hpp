#pragma once

#include "common.hpp"

struct Cell
{
    int lifetime;
    bool hasApple;
};

class Field
{
private:
    Cell fCellField[FIELD_SIZE][FIELD_SIZE];
public:
    Field();
    int createApple();
    void update();
    void delayCellsLifetimeCount();
    int getCellLifetime(int, int) const;
    void setCellLifetime(int, int, int);
    bool getCellAppleInfo(int, int) const;
    void setCellAppleInfo(int, int, bool);
};
