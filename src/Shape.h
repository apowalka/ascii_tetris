#ifndef SHAPE_H
#define SHAPE_H

#include <vector>

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

// Square Shape
// 1 1
// 1 1

// I shape
// 1 1 1 1

// T Shape
// 0 1 0
// 1 1 1

// S Shape
// 1 0
// 1 1
// 0 1

// Z Shape
// 0 1
// 1 1
// 1 0

// L Shape
// 1 0
// 1 0
// 1 1

// J Shape
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
#endif
