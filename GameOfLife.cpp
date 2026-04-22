/*
Author: Peter Samaan
Class: ECE 4122
Last Date Modified: 10/10/25
Description: program entry point
*/
#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <thread>
#include <chrono>
#include <deque>
#include <SFML/Graphics.hpp>
#include "Cell.h"
#ifdef _OPENMP
#endif
using namespace std;

void sequential(vector<vector<Cell>>& grid, int numRows, int numCols)
{
    //execute countNeighbors() and calcState() on each cell
    for (int i = 0; i < numRows; i++)
    {
        for (int j = 0; j < numCols; j++)
        {
            grid[i][j].countNeighbors(grid, i, j, numCols, numRows);
            grid[i][j].calcState();
        }
    }
    //execute updateState() on each cell
    for (int i = 0; i < numRows; i++)
    {
        for (int j = 0; j < numCols; j++)
        {
            grid[i][j].updateState();
        }
    }
}

void multithreaded(vector<vector<Cell>>& grid, int numThreads, int numRows, int numCols)
{
    //lambda function to execute countNeighbors() and calcState() on different regions of grid
    auto worker = [&](int startRow, int endRow)
    {
        for (int i = startRow; i < endRow; i++)
        {
            for (int j = 0; j < numCols; j++)
            {
                grid[i][j].countNeighbors(grid, i, j, numCols, numRows);
                grid[i][j].calcState();
            }
        }
    };
    
    vector<thread> threads;
    int chunkHeight = numRows / numThreads; //create "chunk" for each thread to count/update
    int extra = numRows % numThreads;
    int curr = 0;

    //assign threads to count chunks
    for (int t = 0; t < numThreads; t++)
    {
        int startRow = curr;
        int endRow = startRow + chunkHeight + (t < extra ? 1 : 0); //add extra row to chunk if necessary
        threads.emplace_back(worker, startRow, endRow);
        curr = endRow;
    }
        
    for (auto& t : threads) t.join(); //ensure all counting is complete before updating

    for (int i = 0; i < numRows; i++)
    {
        for (int j = 0; j < numCols; j++)
        {
            grid[i][j].updateState();
        }
    }
}

void OpenMP(vector<vector<Cell>>& grid, int numThreads, int numRows, int numCols)
{
    #ifdef _OPENMP
    #pragma omp parallel for num_threads(numThreads) collapse(4)
    
    //execute countNeighbors() and calcState() on each cell
    for (int i = 0; i < numRows; i++)
    {
        for (int j = 0; j < numCols; j++)
        {
            grid[i][j].countNeighbors(grid, i, j, numCols, numRows);
            grid[i][j].calcState();
        }
    }
    //execute updateState() on each cell
    for (int i = 0; i < numRows; i++)
    {
        for (int j = 0; j < numCols; j++)
        {
            grid[i][j].updateState();
        }
    }

    #else 
        sequential(grid, numRows, numCols);
    #endif
}

int main(int argc, char* argv[])
{
    int numThreads = 8;
    int cellSize = 5;
    int x = 800;
    int y = 600;
    string processType = "THRD";

    //parse command line arguments
    for (int i = 1; i < argc; i++)
    {
        string arg = argv[i];
        switch (arg[1])
        {
            case 'n':
                numThreads = stoi(argv[++i]);
                break;
            case 'c':
                cellSize = stoi(argv[++i]);
                break;
            case 'x':
                x = stoi(argv[++i]);
                break;
            case 'y':
                y = stoi(argv[++i]);
                break;
            case 't':
                processType = argv[++i];
                break;
        }
    }

    const int rows = y / cellSize;
    const int columns = x / cellSize;
    vector<vector<Cell>> cells(rows, vector<Cell>(columns)); //create grid of cells
    
    //randomize each cell state
    random_device rng;
    mt19937 gen(rng());
    bernoulli_distribution chance(0.5);

    for (int i = 0; i < rows; i++) //iterate through 2D vector
    {
        for (int j = 0; j < columns; j++)
        {
            cells[i][j].setState(chance(gen)); //set state to random bool
        }
    }

    deque<long long> genTimes;
    const int samples = 100;
    
    

    sf::RenderWindow window(sf::VideoMode(x, y), "Game of Life");
    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) window.close();
        }

        window.clear(sf::Color::Black);

        sf::RectangleShape square(sf::Vector2f(cellSize, cellSize));
        square.setFillColor(sf::Color::White);

        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {
                if (cells[i][j].getState())
                {
                    square.setPosition(j*cellSize, i*cellSize);
                    window.draw(square);
                }
            }
        }

        window.display();

        auto start = chrono::high_resolution_clock::now(); //start timing generation

        //processType determines proper function to call
        if (processType == "SEQ")
        {
            sequential(cells, rows, columns);
        }

        else if (processType == "THRD")
        {
            multithreaded(cells, numThreads, rows, columns);
        }

        else if (processType == "OMP")
        {
            OpenMP(cells, numThreads, rows, columns);
        }

        auto end = chrono::high_resolution_clock::now(); //stop timing
        auto dur = chrono::duration_cast<chrono::microseconds>(end - start).count(); //store sample

        genTimes.push_back(dur);
        if (genTimes.size() > samples) genTimes.pop_front(); //only keep last 100 samples

        long long sum = 0;
        for (auto i : genTimes) sum += i;

        //processType determines proper function to call
        if (processType == "SEQ")
        {
            cout << "100 generations took " << sum << " microseconds with single thread" << endl;
        }

        else if (processType == "THRD")
        {
            cout << "100 generations took " << sum << " microseconds with " << numThreads << " std::threads" << endl;
        }

        else if (processType == "OMP")
        {
            cout << "100 generations took " << sum << " microseconds with " << numThreads << " OMP threads" << endl;
        }
    }
}
