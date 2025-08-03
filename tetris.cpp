#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <vector>
#include <stack>
#include <queue>
#include <list>
#include <unordered_map>
#include <cstdint>
#include <limits>
#include <unordered_set>
#include <algorithm>
#include <unistd.h>

// random
#include <random>
// for _kbhit()
#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
//#include <stropts.h>
//#include <asm/termbits.h>
#include <sys/ioctl.h>

int _kbhit() {
    static const int STDIN = 0;
    static bool initialized = false;
 
    if (! initialized) {
        // Use termios to turn off line buffering
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }
 
    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}
 
using namespace std;

class Shape
{
public:
    Shape(std::vector<std::vector<int>> pattern)
    {
        shapeGrid_ = pattern;
    }
    int rows() {return shapeGrid_.size();}
    int cols() {return shapeGrid_.at(0).size();}

    void rotate()
    {
        const int currRows = rows();
        const int currCols = cols();
        std::vector<std::vector<int>> newShapeGrid;
        newShapeGrid.resize(currCols);
        for (int i = 0; i < newShapeGrid.size(); ++i)
        {
            newShapeGrid.at(i).resize(currRows);    
        }
        

        for (int i = 0; i < currRows; ++i)
        {
            for (int j = 0; j < currCols; ++j)
            {
                newShapeGrid.at(j).at(currRows - 1 - i) = shapeGrid_.at(i).at(j);
            }
        }

        shapeGrid_ = newShapeGrid;
    }
    std::vector<std::vector<int>> getGrid(){ return shapeGrid_; }
    virtual ~Shape(){}

private:
    std::vector<std::vector<int>> shapeGrid_;
};

// 1 1
// 1 1

// 1 1 1 1

// 0 1 0
// 1 1 1

// 1 0
// 1 1
// 0 1

// 0 1
// 1 1
// 1 0

// 1 0
// 1 0
// 1 1

// 0 1
// 0 1
// 1 1

class SquareShape : public Shape
{
public:
    SquareShape()
        : Shape({ {1, 1},{1, 1} })
    {
    }
    ~SquareShape(){}
};

class JShape : public Shape
{
public:
    JShape()
        : Shape({ {0, 1},{0, 1},{1, 1} })
    {
    }
    ~JShape(){}
};

class LShape : public Shape
{
public:
    LShape()
        : Shape({ {1, 0},{1, 0},{1, 1} })
    {
    }
    ~LShape(){}
};

class ZShape : public Shape
{
public:
    ZShape()
        : Shape({ {1, 1, 0},{0, 1, 1} })
    {
    }
    ~ZShape(){}
};

class SShape : public Shape
{
public:
    SShape()
        : Shape({ {0, 1, 1},{1, 1, 0} })
    {
    }
    ~SShape(){}
};

class TShape : public Shape
{
public:
    TShape()
        : Shape({ {0, 1, 0},{1, 1, 1} })
    {
    }
    ~TShape(){}
};

class IShape : public Shape
{
public:
    IShape()
        : Shape({ {1} , {1}, {1}, {1} })
    {
    }
    ~IShape(){}
};

class Grid
{
public:
    Grid(int rows, int cols)
        :rows_(rows), cols_(cols)
    {
        createGrid(grid_, rows_, cols_);
        setInitialPattern();
        generateNewShape();
    }

    void generateNewShape()
    {
        int shapeNum = rand() % 8;
        if (shapeNum == 0)
        {
            currShape_ = new SquareShape();
        }
        else if (shapeNum == 1)
        {
            currShape_ = new JShape();
        }
        else if (shapeNum == 2)
        {
            currShape_ = new LShape();
        }
        else if (shapeNum == 3)
        {
            currShape_ = new TShape();
        }
        else if (shapeNum == 4)
        {
            currShape_ = new TShape();
        }
        else if (shapeNum == 5)
        {
            currShape_ = new ZShape();
        }
        else if (shapeNum == 6)
        {
            currShape_ = new SShape();
        }
        else if (shapeNum == 7)
        {
            currShape_ = new IShape();
        }
        currentShapePosition_ = { 0, cols_/2};
        drawShape();
    }

    void drawShape()
    {
        int currR = currentShapePosition_.first; 
        int currC = currentShapePosition_.second; 
        std::vector<std::vector<int>> shapeGrid = currShape_->getGrid();

        // loop over shape grid
        for (int i = 0; i < currShape_->rows() ; ++i)
        {
            for (int j = 0; j < currShape_->cols(); ++j)
            {
                if (shapeGrid.at(i).at(j) == 1)
                {
                    grid_[currR + i][currC + j] = shapeGrid.at(i).at(j);
                }
            }
        }
    }

    // TODO needs work
    bool isDownMovementBlocked()
    {
        int currR = currentShapePosition_.first; 
        int currC = currentShapePosition_.second; 
        std::vector<std::vector<int>> shapeGrid = currShape_->getGrid();
        // check last row of shape and if one row down on grid is open

        vector<int> lastShapeRow = shapeGrid.at(shapeGrid.size() - 1);
        for (int j = 0; j < currShape_->cols(); ++j)
        {
            else if (lastShapeRow.at(j) == 1 
                && grid_[currR + shapeGrid.size() - 1 + 1][currC + j] == 1)
            {
                return true;
            }
        }
        return false;
    }

    void moveShape(std::pair<int,int> unitDir)
    {
        if (!isDownMovementBlocked())
        {
            std::pair<int, int> currPos = currentShapePosition_;
            clearShape();
            
            currentShapePosition_ 
                = {currPos.first + unitDir.first, currPos.second + unitDir.second};
            drawShape();
        }
        else
        {
            generateNewShape();
        }
    }

    void clearShape()
    {
        int currR = currentShapePosition_.first; 
        int currC = currentShapePosition_.second; 
        std::vector<std::vector<int>> shapeGrid = currShape_->getGrid();

        // loop over shape grid
        for (int i = 0; i < currShape_->rows() ; ++i)
        {
            for (int j = 0; j < currShape_->cols(); ++j)
            {
                if (shapeGrid.at(i).at(j) == 1)
                {
                    grid_[currR + i][currC + j] = 0;
                }
            }
        }
    }

    void rotateShape()
    {
        clearShape();
        currShape_->rotate();
        drawShape();
    }

    ~Grid()
    {
        for(int k = 0; k < rows_; ++k)
        {
            delete[] grid_[k];
        }
        delete[] grid_;
    }

    //void copyGrid(int** sourceGrid, int** destGrid, int rows, int cols)
    //{
    //    for (int i = 0; i < rows; ++i)
    //    {
    //        for (int j = 0; j < cols; ++j)
    //        {
    //            destGrid[i][j] = sourceGrid[i][j];
    //        }
    //    }
    //    
    //}

    bool updateShape(char dir)
    {
        std::pair<int, int> pos = currentShapePosition_;
        if (dir == 'a') // move left
        {
            moveShape({0, -1});      
        }
        else if (dir == 'd')
        {
            moveShape({0, 1});      
        }
        else if (dir == 's')
        {
            moveShape({1, 0});
        }
        else if (dir == 'm') // rotate
        {
            rotateShape();
        }

        return true;   
    }
    
    void printGrid()
    {
        for (int i = 0; i < cols_; ++i)
        {
            cout << "_";
        }
        cout << endl;
        for (int i = 0; i < rows_; ++i)
        {
            cout << "|";
            for (int j = 0; j < cols_; ++j)
            {
                if (grid_[i][j] == 1)
                {
                    cout << "X";
                }
                else
                {
                    cout << " ";
                }
            }
            cout << "|" << endl;
        }
    }

private:
    void createGrid(int**& grid, int rows, int cols)
    {
        grid = new int*[rows];  
        for (int i = 0; i < rows; ++i)
        {
            grid[i] = new int[cols]();
        }
    }

    void setInitialPattern()
    {
    
        for (int i = rows_ - 5; i < rows_; ++i)
        {
            for (int j = 0; j < cols_; ++j)
            {
                grid_[i][j] = rand() % 2;
            }
        }
    }

private:
    int** grid_;
    int rows_;
    int cols_;
    Shape* currShape_;
    std::pair<int, int> currentShapePosition_;
};

void run()
{
    int rows = 50;
    int cols = 50;
    Grid* myGrid = new Grid(rows, cols);
    while (1)
    {
        myGrid->printGrid();
        //enforceRules(grid, rows, cols);
        if (_kbhit())
        {
            char dir = getchar();
            myGrid->updateShape(dir);
        }
        // always move down
        //myGrid->updateShape('s');
        usleep(10000);
        system("clear");
    };
}

int main()
{
    run();
    return 0;
}
