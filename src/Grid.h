#ifndef GRID_H
#define GRID_H
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <unistd.h>

#include "Shape.h"

using namespace std;

class Grid
{
public:
    Grid(int rows, int cols);
    ~Grid();
    bool updateShape(char dir);
    void printGrid();
    void clearFilledLines();

private:
    void createGrid(int**& grid, int rows, int cols);
    void setInitialPattern();
    int getShapeWidth();
    void generateNewShape();
    void drawShape();
    void moveShape(std::pair<int,int> unitDir);
    void clearShape();
    void rotateShape();
    bool isRotateBlocked();
    bool isMovementBlocked(std::pair<int,int> unitDir);

    int** grid_;
    int rows_;
    int cols_;
    Shape* currShape_;
    std::pair<int, int> currentShapePosition_;
};

#endif

