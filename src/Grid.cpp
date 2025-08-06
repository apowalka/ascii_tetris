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
    drawShape();
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

// TODO block when moving to the left into grid[i][j]=1
bool Grid::isLeftMovementBlocked()
{
    int currR = currentShapePosition_.first; 
    int currC = currentShapePosition_.second; 
    if (currC - 1 < 0)
    {
        return true;
    }

    // check left most column of the shape
    std::vector<std::vector<int>> shapeGrid = currShape_->getGrid();
    for (int i = 0; i < shapeGrid.size(); ++i)
    {
        if (shapeGrid[i][0] == 1)
        {
            if (grid_[currR + i][currC - 1] == 1)
            {
                return true;
            }
            break;
        }
    }

    return false;
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

// TODO block when moving to the right into grid[i][j]=1
bool Grid::isRightMovementBlocked()
{
    int currR = currentShapePosition_.first; 
    int currC = currentShapePosition_.second; 

    // make sure we can't leave the grid
    int currShapeWidth = getShapeWidth();
    if (currC + currShapeWidth == cols_)
    {
        return true;
    }

    return false;
}

bool Grid::isDownMovementBlocked()
{
    int currR = currentShapePosition_.first; 
    int currC = currentShapePosition_.second; 
    std::vector<std::vector<int>> shapeGrid = currShape_->getGrid();

    for (int j = 0; j < currShape_->cols(); ++j)
    {
        for (int i = currShape_->rows() - 1; i >= 0; --i)
        {
            if (shapeGrid[i][j] == 1)
            {
                if (grid_[currR + i + 1][currC + j] == 1)
                {
                    return true;
                }
                break;
            }
        }
    }
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
    std::pair<int, int> pos = currentShapePosition_;
    if (dir == 'a') // move left
    {
        if (!isLeftMovementBlocked())
        {
            moveShape({0, -1});      
        }
    }
    else if (dir == 'd')
    {
        if (!isRightMovementBlocked())
        {
            moveShape({0, 1});      
        }
    }
    else if (dir == 's')
    {
        if (!isDownMovementBlocked())
        {
            moveShape({1, 0});
        }
        else
        {
            generateNewShape();
        }
    }
    else if (dir == 'm') // rotate
    {
        rotateShape();
    }

    return true;   
}

void Grid::clearFilledLines()
{
    for (int i = 0; i < rows_; ++i)
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
            for (int j = 0; j < cols_; ++j)
            {
                grid_[i][j] = 0;
            }
        }
    }
}

void Grid::printGrid()
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
