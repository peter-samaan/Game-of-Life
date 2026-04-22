/*
Author: Peter Samaan
Class: ECE 4122
Last Date Modified: 10/10/25
Description: interface for Cell class
*/
#include <vector>
using namespace std;

class Cell
{
public:
    void countNeighbors(const vector<vector<Cell>>&, int, int, int, int); //return number of living neighbor cells
    void calcState(); //determines whether cell is alive after each generation
    void updateState(); //changes state to next state

    //getter and setter functions
    bool getState();
    bool getNextState();
    int getNeighbors();
    void setState(bool);
    void setNextState(bool);
    void setNeighbors(int);

private:
    bool state, nextState;
    int livingNeighbors;
};