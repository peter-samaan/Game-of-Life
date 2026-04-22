/*
Author: Peter Samaan
Class: ECE 4122
Last Date Modified: 10/10/25
Description: implementation for Cell class
*/
#include "Cell.h"
using namespace std;

void Cell::countNeighbors(const vector<vector<Cell>>& grid, int x, int y, int width, int height)
{
    livingNeighbors = 0; //reset neighbor count each generation

    for (int i = -1; i <= 1; i++) //nested loop that views 3x3 grid around "center" cell
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0) continue; //skip center cell

            int newX = x + i;
            int newY = y + j;
            if (newX >= 0 && newX < height && newY >= 0 && newY < width)
            {
                livingNeighbors += grid[newX][newY].state;
            }
        }
    }
}

void Cell::calcState()
{
    if (state)
    {
        if (livingNeighbors == 2 || livingNeighbors == 3) nextState = true;
        else nextState = false;
    }

    else
    {
        if (livingNeighbors == 3) nextState = true;
        else nextState = false;
    }
}

void Cell::updateState()
{
    state = nextState;
}

bool Cell::getState()
{
    return state;
}

bool Cell::getNextState()
{
    return nextState;
}

int Cell::getNeighbors()
{
    return livingNeighbors;
}

void Cell::setState(bool b)
{
    state = b;
}

void Cell::setNextState(bool b)
{
    nextState = b;
}

void Cell::setNeighbors(int n)
{
    livingNeighbors = n;
}