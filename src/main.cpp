#include "Grid.h"
#include "Shape.h"
#include "Utility.h"
#include <thread>

void captureInput(Grid* myGrid)
{
    while (!myGrid->isGameOver())
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
    while (!myGrid->isGameOver())
    {
        usleep(10000);
        system("clear");
        myGrid->printGrid();
    };
}

void moveDown(Grid* myGrid)
{
    int usSleep = 1000000;
    const unsigned int increment = 10;
    unsigned int nextLevelScore = increment;
    while (!myGrid->isGameOver())
    {
        myGrid->updateShape('s');
        usleep(usSleep);
        if (myGrid->getScore() >= nextLevelScore)
        {
            usSleep *= 0.6;
            nextLevelScore += increment;
        }
    };
}

int main()
{
    int rows = 20;
    int cols = 10;
    Grid* myGrid = new Grid(rows, cols);

    std::thread capture(captureInput, myGrid);
    std::thread refresh(refreshScreen, myGrid);
    std::thread down(moveDown, myGrid);
    capture.join();
    refresh.join();
    down.join();
    cout << "Game Over" << endl;
    
    return 0;
}
