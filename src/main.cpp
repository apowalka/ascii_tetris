#include "Grid.h"
#include "Shape.h"
#include "Utility.h"

int main()
{
    int rows = 50;
    int cols = 50;
    Grid* myGrid = new Grid(rows, cols);
    while (true)
    {
        myGrid->printGrid();
        if (Utility::_kbhit())
        {
            char dir = getchar();
            myGrid->updateShape(dir);
        }
        // always move down
        //myGrid->updateShape('s');
        usleep(10000);
        system("clear");
    };
    return 0;
}
