#include <random>
#include <vector>
#include <memory>
#include "Grid.h"

Grid::Grid(int rows, int cols)
    :rows_(rows), cols_(cols)
{
    createGrid(grid_, rows_, cols_);
    setInitialPattern();
    generateNewShape();
    playNewShape();
    srand (time(NULL));
    gameId_ = rand() % 10000;
}

Grid::~Grid()
{
}

void Grid::generateNewShape()
{
    int shapeNum = rand() % 7;
    std::unique_ptr<Shape> newShape;
    if (shapeNum == 0)
    {
        newShape = std::make_unique<SquareShape>();
    }
    else if (shapeNum == 1)
    {
        newShape = std::make_unique<JShape>();
    }
    else if (shapeNum == 2)
    {
        newShape = std::make_unique<LShape>();
    }
    else if (shapeNum == 3)
    {
        newShape = std::make_unique<TShape>();
    }
    else if (shapeNum == 4)
    {
        newShape = std::make_unique<ZShape>();
    }
    else if (shapeNum == 5)
    {
        newShape = std::make_unique<SShape>();
    }
    else //if (shapeNum == 6)
    {
        newShape = std::make_unique<IShape>();
    }
    shapeQueue_.push(*newShape);
}

// TODO there is a bug where completing a new line will move the next shape up on the opponents screen.
// sol: before checking for completed lines, clear the next shape and the write it back in
void Grid::playNewShape()
{
    // add new shape to shape queue
    generateNewShape();

    // clear the old next shape from grid since it will become current shape
    if (nextShapePosition_.first != -1)
    {
        clearShape(nextShapePosition_.first , nextShapePosition_.second, nextShape_);
    }

    // get new shape to play from front of queue
    currShape_ = shapeQueue_.front();
    shapeQueue_.pop();

    // set next shape as the following shape in the queue
    nextShape_ = shapeQueue_.front();
    nextShapePosition_ = { 0, cols_ / 2};

    // draw next shape on grid
    draw(nextShapePosition_.first, nextShapePosition_.second, nextShape_);

    currentShapePosition_ = { ROW_START, cols_ / 2};
    if (canPlaceShape())
    {
        drawCurrentShape();
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
    const std::vector<std::vector<int>>& shapeGrid = currShape_.getGrid();

    // loop over shape grid
    for (int i = 0; i < currShape_.rows() ; ++i)
    {
        for (int j = 0; j < currShape_.cols(); ++j)
        {
            if (shapeGrid.at(i).at(j) == 1 && grid_[currR + i][currC + j] == 1)
            {
                return false;
            }
        }
    }
    return true;
}

void Grid::drawCurrentShape()
{
    int currR = currentShapePosition_.first; 
    int currC = currentShapePosition_.second; 
    draw(currR, currC, currShape_);
}

void Grid::drawNextShape()
{
    draw(nextShapePosition_.first, nextShapePosition_.second, nextShape_);
}

void Grid::draw(int row, int col, const Shape& myShape)
{
    const std::vector<std::vector<int>>& shapeGrid = myShape.getGrid();

    // loop over shape grid
    const int rows = myShape.rows();
    const int cols = myShape.cols();
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            if (shapeGrid.at(i).at(j) == 1)
            {
                grid_[row + i][col + j] = shapeGrid.at(i).at(j);
            }
        }
    }
}
/* deprecated
int Grid::getShapeWidth()
{
    // go through each row and count the number of 1's
    // use the max amount
    int maxWidth = 0;
    const std::vector<std::vector<int>>& shapeGrid = currShape_.getGrid();
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
*/

// clear shape from grid and determine if we can make the move in the unit direction
// then redraw the shapea back where it was originally
bool Grid::isMovementBlocked(std::pair<int,int> unitDir)
{
    int rows = currShape_.rows();
    int cols = currShape_.cols();
    auto shapeGrid = currShape_.getGrid();

    int currR = currentShapePosition_.first + unitDir.first; 
    int currC = currentShapePosition_.second + unitDir.second; 

    clearCurrentShape();
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            if (shapeGrid[i][j] == 1)
            {
                if ( currR + i > rows_-1 || currC + j > cols_-1 || currC + j < 0
                    || grid_[currR + i][currC + j] == 1)
                {
                    drawCurrentShape();
                    return true;
                }
            }
        }
    }   
    
    drawCurrentShape();
    return false;
}

bool Grid::isRotateBlocked()
{
    // make copy to test rotation
    Shape myShape(currShape_);
    myShape.rotate();
    int rows = myShape.rows();
    int cols = myShape.cols();
    auto shapeGrid = myShape.getGrid();

    int currR = currentShapePosition_.first; 
    int currC = currentShapePosition_.second; 

    clearCurrentShape();
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            if (shapeGrid[i][j] == 1)
            {
                if ( currR + i > rows_-1 || currC + j > cols_-1 || currC + j < 0
                    || (grid_[currR + i][currC + j] == 1))
                {
                    drawCurrentShape();
                    return true;
                }
            }
        }
    }

    drawCurrentShape();
    return false;
}

void Grid::moveShape(std::pair<int,int> unitDir)
{
    std::pair<int, int> currPos = currentShapePosition_;
    clearCurrentShape();
    
    currentShapePosition_ 
        = {currPos.first + unitDir.first, currPos.second + unitDir.second};
    drawCurrentShape();
}

void Grid::clearCurrentShape()
{
    int currR = currentShapePosition_.first; 
    int currC = currentShapePosition_.second; 
    clearShape(currR, currC, currShape_);
}

void Grid::clearNextShape()
{
    clearShape(nextShapePosition_.first, nextShapePosition_.second, nextShape_);
}

void Grid::clearShape(int row, int col, const Shape& myShape)
{
    const std::vector<std::vector<int>>& shapeGrid = myShape.getGrid();

    // loop over shape grid
    const int rows = myShape.rows();
    const int cols = myShape.cols();
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            if (shapeGrid.at(i).at(j) == 1)
            {
                grid_[row + i][col + j] = 0;
            }
        }
    }
}

void Grid::rotateShape()
{
    clearCurrentShape();
    currShape_.rotate();
    drawCurrentShape();
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
            playNewShape();
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
    for (int i = rows_ - 1; i >= ROW_START; --i)
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
            clearNextShape();
            ++score_; 
            ++linesFilled_;
            for (int j = 0; j < cols_; ++j)
            {
                grid_[i][j] = 0;
            }
            //usleep(100000);
            //for (int ii = i; ii - 1 >= ROW_START; --ii)
            for (int ii = i; ii - 1 >= 0; --ii)
            {
                for (int jj = 0; jj < cols_; ++jj)
                {
                    grid_[ii][jj] = grid_[ii-1][jj];
                }
            }
            // go back and check the line we copied into the cleared line memory
            ++i;
            drawNextShape();
        }
    }
}

void Grid::addPenaltyLines(unsigned int lines)
{
    std::lock_guard<std::mutex> guard(myMutex);
    // need to clear shape. It would also get moved up otherwise.
    clearCurrentShape();
    clearNextShape();

    // check if moving up causes game to end
    for (int i = 0; i < lines; ++i)
    {
        for (int j = 0; j < cols_; ++j)
        {
            if (grid_[i][j] == 1)
            {
                endGame();
                break;
            }
        }
        if (isGameOver_ == true)
        {
            break;
        }
    }

    // move everything except for the shape up
    for (int i = 0; i < rows_; ++i)
    {
        for (int j = 0; j < cols_; ++j)
        {
            if (i < rows_ - lines)
            {
                grid_[i][j] = grid_[i + lines][j];
            }
            else
            {
                //grid_[i][j] = rand() % 2;
                grid_[i][j] = 2;
            }
        }
    }

    // TODO what if there is a 1's in any of the spots we just redrew in
    drawCurrentShape();
    drawNextShape();
}

GameInfo Grid::getGameInfo()
{
    std::lock_guard<std::mutex> guard(myMutex);
    GameInfo info;
    info.grid = grid_;
    info.rows = rows_;
    info.cols = cols_;
    info.score = score_;
    info.linesFilled = linesFilled_;
    info.isGameOver = isGameOver_;
    info.gameId = gameId_;
    return info;
}

void Grid::createGrid(std::vector<std::vector<int>>& grid, int rows, int cols)
{
    grid.reserve(rows);
    for (int i = 0; i < rows; ++i)
    {
        std::vector<int> row(cols, 0);
        grid.push_back(row);
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

/*
unsigned int Grid::getLinesFilled()
{
    unsigned int count = linesFilled_;
    linesFilled_ = 0;
    return count;
}
*/
void Grid::clearLinesFilled()
{
    std::lock_guard<std::mutex> guard(myMutex);
    linesFilled_ = 0;
}
