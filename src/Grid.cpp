#include <random>
#include <vector>
#include "Grid.h"

Grid::Grid(int rows, int cols)
    :rows_(rows), cols_(cols)
{
    createGrid(grid_, rows_, cols_);
    setInitialPattern();
    generateNewShape();
}

Grid::~Grid()
{
    for(int k = 0; k < rows_; ++k)
    {
        delete[] grid_[k];
    }
    delete[] grid_;
}

void Grid::generateNewShape()
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
    if (canPlaceShape())
    {
        drawShape();
    }
    else
    {
        endGame();
    }
}

void Grid::endGame()
{
    isGameOver_ = true;
}

bool Grid::canPlaceShape()
{
    int currR = currentShapePosition_.first; 
    int currC = currentShapePosition_.second; 
    std::vector<std::vector<int>> shapeGrid = currShape_->getGrid();

    // loop over shape grid
    for (int i = 0; i < currShape_->rows() ; ++i)
    {
        for (int j = 0; j < currShape_->cols(); ++j)
        {
            if (shapeGrid.at(i).at(j) == 1 && grid_[currR + i][currC + j] == 1)
            {
                return false;
            }
        }
    }
    return true;
}

void Grid::drawShape()
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

int Grid::getShapeWidth()
{
    // go through each row and count the number of 1's
    // use the max amount
    int maxWidth = 0;
    std::vector<std::vector<int>> shapeGrid = currShape_->getGrid();
    for (int i = 0; i < shapeGrid.size(); ++i)
    {
        int tmpWidth = 0;
        for (int j = 0; j < shapeGrid.at(i).size(); ++j)
        {
            if (shapeGrid.at(i).at(j) == 1)
            {
                ++tmpWidth; 
            }
        }
        if (tmpWidth > maxWidth)
        {
            maxWidth = tmpWidth;
        }
    }
    return maxWidth;
}

// clear shape from grid and determine if we can make the move in the unit direction
// then redraw the shapea back where it was originally
bool Grid::isMovementBlocked(std::pair<int,int> unitDir)
{
    int rows = currShape_->rows();
    int cols = currShape_->cols();
    auto shapeGrid = currShape_->getGrid();

    int currR = currentShapePosition_.first + unitDir.first; 
    int currC = currentShapePosition_.second + unitDir.second; 

    clearShape();
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            if (shapeGrid[i][j] == 1)
            {
                if ( currR + i > rows_-1 || currC + j > cols_-1 || currC + j < 0
                    || grid_[currR + i][currC + j] == 1)
                {
                    drawShape();
                    return true;
                }
            }
        }
    }   
    
    drawShape();
    return false;
}

bool Grid::isRotateBlocked()
{
    // make copy to test rotation
    Shape myShape(*currShape_);
    myShape.rotate();
    int rows = myShape.rows();
    int cols = myShape.cols();
    auto shapeGrid = myShape.getGrid();

    int currR = currentShapePosition_.first; 
    int currC = currentShapePosition_.second; 

    clearShape();
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            if (shapeGrid[i][j] == 1)
            {
                if ( currR + i > rows_-1 || currC + j > cols_-1 || currC + j < 0
                    || (grid_[currR + i][currC + j] == 1))
                {
                    drawShape();
                    return true;
                }
            }
        }
    }

    drawShape();
    return false;
}

void Grid::moveShape(std::pair<int,int> unitDir)
{
    std::pair<int, int> currPos = currentShapePosition_;
    clearShape();
    
    currentShapePosition_ 
        = {currPos.first + unitDir.first, currPos.second + unitDir.second};
    drawShape();
}

void Grid::clearShape()
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

void Grid::rotateShape()
{
    clearShape();
    currShape_->rotate();
    drawShape();
}


//void Grid::copyGrid(int** sourceGrid, int** destGrid, int rows, int cols)
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

bool Grid::updateShape(char dir)
{
    std::lock_guard<std::mutex> guard(myMutex);
    std::pair<int, int> pos = currentShapePosition_;
    if (dir == 'a') // move left
    {
        if (!isMovementBlocked({0, -1}))
        {
            moveShape({0, -1});      
        }
    }
    else if (dir == 'd')
    {
        if (!isMovementBlocked({0, 1}))
        {
            moveShape({0, 1});      
        }
    }
    else if (dir == 's')
    {
        if (!isMovementBlocked({1, 0}))
        {
            moveShape({1, 0});
        }
        else
        {
            clearFilledLines();
            generateNewShape();
        }
    }
    else if (dir == 'm') // rotate
    {
        if (!isRotateBlocked())
        {
            rotateShape();
        }
        // attempt wall kick
        else
        {
            if (!isMovementBlocked({0, -1}))
            {
                moveShape({0, -1});
                if (!isRotateBlocked()) 
                {
                    rotateShape();
                }
                else
                {
                    // move shape back
                    moveShape({0, 1});
                }
            }
            else if (!isMovementBlocked({0, 1}))
            {
                moveShape({0, 1});
                if (!isRotateBlocked()) 
                {
                    rotateShape();
                }
                else
                {
                    // move shape back
                    moveShape({0, -1});
                }
            }
        }

    }

    return true;   
}

void Grid::clearFilledLines()
{
    for (int i = rows_ - 1; i >= 0; --i)
    {
        bool isFilled = true;
        for (int j = 0; j < cols_; ++j)
        {
            if (grid_[i][j] != 1)
            {
                isFilled = false;
                break;
            }
        }
        // clear row
        if (isFilled)
        {
            ++score_; 
            for (int j = 0; j < cols_; ++j)
            {
                grid_[i][j] = 0;
            }
            //usleep(100000);
            for (int ii = i; ii - 1 >= 0; --ii)
            {
                for (int jj = 0; jj < cols_; ++jj)
                {
                    grid_[ii][jj] = grid_[ii-1][jj];
                }
            }
            // go back and check the line we copied into the clearned line memory
            ++i;
        }
    }
}

std::pair<int**, int> Grid::getGameInfo()
{
    std::lock_guard<std::mutex> guard(myMutex);
    return {grid_, score_};
}

void Grid::createGrid(int**& grid, int rows, int cols)
{
    grid = new int*[rows];  
    for (int i = 0; i < rows; ++i)
    {
        grid[i] = new int[cols]();
    }
}

void Grid::setInitialPattern()
{

    for (int i = rows_ - 5; i < rows_; ++i)
    {
        for (int j = 0; j < cols_; ++j)
        {
            grid_[i][j] = rand() % 2;
        }
    }
}

bool Grid::isGameOver()
{
    std::lock_guard<std::mutex> guard(myMutex);
    return isGameOver_;
}

unsigned int Grid::getScore()
{
    std::lock_guard<std::mutex> guard(myMutex);
    return score_;
}
