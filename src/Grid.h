#ifndef GRID_H
#define GRID_H
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <mutex>
#include <cstdint>
#include <algorithm>
#include <utility>
#include <unistd.h>
#define NCURSES_NOMACROS
#include <ncurses.h>

#include "Shape.h"

using namespace std;

class Grid
{
public:
    Grid(int rows, int cols);
    ~Grid();
    bool updateShape(char dir);
    bool isGameOver();
    unsigned int getScore();
    std::pair<int**, int> getGameInfo();
    int getRows(){return rows_;};
    int getCols(){return cols_;};

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
    void clearFilledLines();
    bool canPlaceShape();
    void endGame();

    int** grid_;
    int rows_;
    int cols_;
    Shape* currShape_;
    std::pair<int, int> currentShapePosition_;
    std::mutex myMutex;
    bool isGameOver_ = false;
    unsigned int score_ = 0;
};

#endif

