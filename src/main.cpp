#include "Grid.h"
#include "Shape.h"
#include "Utility.h"
#include <thread>

void captureInput(Grid* myGrid)
{
    while (true)
    {
        if (Utility::_kbhit())
        {
            char dir = getchar();
            myGrid->updateShape(dir);
        }
        usleep(10000);
    };
}

void refreshScreen(Grid* myGrid)
{
    while (true)
    {
        myGrid->printGrid();
        usleep(10000);
        system("clear");
    };
}

void moveDown(Grid* myGrid)
{
    while (true)
    {
        myGrid->updateShape('s');
        usleep(1000000);
    };
}

int main()
{
    int rows = 20;
    int cols = 25;
    Grid* myGrid = new Grid(rows, cols);

    std::thread capture(captureInput, myGrid);
    std::thread refresh(refreshScreen, myGrid);
    std::thread down(moveDown, myGrid);
    capture.join();
    refresh.join();
    down.join();
    
    return 0;
}
