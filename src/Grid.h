#ifndef GRID_H
#define GRID_H
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <mutex>
#include <cstdint>
#include <algorithm>
#include <tuple>
#include <unistd.h>
#define NCURSES_NOMACROS
#include <ncurses.h>

#include "Shape.h"

using namespace std;

struct GameInfo
{
    std::vector<std::vector<int>> grid;
    int rows = 0;
    int cols = 0;
    unsigned int score = 0;
    unsigned int linesFilled = 0;
    bool isGameOver = false;
    unsigned int gameId = 0;
};

class Grid
{
public:
    Grid(int rows, int cols);
    ~Grid();
    bool updateShape(char dir);
    bool isGameOver();
    unsigned int getScore();
    GameInfo getGameInfo();
    int getRows(){return rows_;};
    int getCols(){return cols_;};
    void addPenaltyLines(unsigned int lines);
    void clearLinesFilled();

private:
    void createGrid(std::vector<std::vector<int>>& grid, int rows, int cols);
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
//    unsigned int getLinesFilled();

    std::vector<std::vector<int>> grid_;
    int rows_;
    int cols_;
    Shape* currShape_;
    std::pair<int, int> currentShapePosition_;
    std::mutex myMutex;
    bool isGameOver_ = false;
    unsigned int score_ = 0;
    unsigned int linesFilled_ = 0;
    unsigned int gameId_ = 0;
};

#endif

