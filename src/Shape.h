#ifndef SHAPE_H
#define SHAPE_H

#include <vector>

class Shape
{
public:
    Shape()
    {
    }

    Shape(std::vector<std::vector<int>> pattern)
    {
        shapeGrid_ = pattern;
    }

    Shape(const Shape& myShape)
        :shapeGrid_(myShape.getGrid())
    {
    }

    Shape operator=(const Shape& myShape)
    {
        if (this == &myShape)
        {
            return *this;
        }
        shapeGrid_ = myShape.getGrid();
        return *this;
    }

    virtual ~Shape(){}

    int rows() const { return shapeGrid_.size(); }
    int cols() const { return shapeGrid_.at(0).size(); }
    std::vector<std::vector<int>> getGrid() const { return shapeGrid_; }

    void rotate()
    {
        std::reverse(shapeGrid_.begin(), shapeGrid_.end());
        for (int i = 0; i < shapeGrid_.size(); ++i)
        {
            for (int j = i + 1; j < shapeGrid_[i].size(); ++j)
            {
                std::swap(shapeGrid_[i][j], shapeGrid_[j][i]);
            }
        }
    }

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
        : Shape({ {0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0} })
    {
    }
    ~SquareShape(){}
};

class JShape : public Shape
{
public:
    JShape()
        : Shape({ {0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 1}, {0,0,0,0} })
    {
    }
    ~JShape(){}
};

class LShape : public Shape
{
public:
    LShape()
        : Shape({ {0, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 0, 0}, {0, 0, 0, 0} })
    {
    }
    ~LShape(){}
};

class ZShape : public Shape
{
public:
    ZShape()
        : Shape({ {0, 0, 0, 0}, {0, 1, 1, 0},{0, 0, 1, 1}, {0,0,0,0} })
    {
    }
    ~ZShape(){}
};

class SShape : public Shape
{
public:
    SShape()
        : Shape({ {0, 0, 0, 0}, {0, 0, 1, 1}, {0, 1, 1, 0}, {0,0,0,0} })
    {
    }
    ~SShape(){}
};

class TShape : public Shape
{
public:
    TShape()
        : Shape({ {0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 1, 0}, {0,0,0,0} })
    {
    }
    ~TShape(){}
};

class IShape : public Shape
{
public:
    IShape()
        : Shape({ {0,0,0,0} , {1,1,1,1}, {0,0,0,0}, {0,0,0,0} })
    {
    }
    ~IShape(){}
};
#endif
