/* Name: Xiaoyu Cheng
 * Course: CS 106B
 * Section: Thursday 3:30pm
 * Description: This file implements a simple game called "Game of Life".
 * Sources: stanford libirary documentation; cplusplus.com (C++ standard library documentation)
 *          https://bitstorm.org/gameoflife/ - Simulation software, used to help create mycolony.txt
 */


#include <iostream>
#include "console.h"
#include "lifegui.h"
#include "grid.h"
#include "vector.h"
#include <string>
#include <fstream>
#include "filelib.h"
#include "simpio.h"
#include "console.h"
#include "strlib.h"

using namespace std;

void readFileToGrid(Grid<string>& g);
void displayGrid(const Grid<string>& g, int rows, int cols);
string tickSingleCell(Grid<string>& g, int rowInd, int colInd, int rows, int cols);
void tickGrid(Grid<string>& g, int rows, int cols);
void updateGUI(const Grid<string>& g, int rows, int cols);


int main() {
    //animation - is this what we are supposed to do?
    //style questions: Is main() readFileToGrid() too long? Should I seperate readFileToGrid to two methods?
    //if choose load another file, should we print out the begining welcomes?
    //what to write if didn't encounter major bugs


    cout << "Welcome to the CS 106B/X Game of Life!" << endl;
    cout << "This program simulates the lifecycle of a bacterial colony." << endl;
    cout << "Cells (X) live and die by the following rules:" << endl;
    cout << "* A cell with 1 or fewer neighbors dies." << endl;
    cout << "* Locations with 2 neighbors remain stable." << endl;
    cout << "* Locations with 3 neighbors will create life." << endl;
    cout << "* A cell with 4 or more neighbors dies. \n" << endl;

    LifeGUI::initialize();
    bool loadAnother = true;

    while (loadAnother){
        Grid<string> board(1,1);
        readFileToGrid(board);

        int row = board.numRows();
        int col = board.numCols();

        displayGrid(board, row, col);

        LifeGUI::resize(row, col);
        updateGUI(board, row, col);


        string option = "initial option";

        while (option != "q"){
            option = toLowerCase(getLine("a)nimate, t)ick, q)uit? "));

            if (option == "t" || option == ""){
                tickGrid(board, row, col);
                displayGrid(board, row, col);
                updateGUI(board, row, col);
            } else if (option == "a" ){
                int numFrames = getInteger("How many frames? ");
                for (int i = 0; i < numFrames; i++){
                    pause(100);
                    clearConsole();
                    tickGrid(board, row, col);
                    displayGrid(board, row, col);
                    updateGUI(board, row, col);
                }
            } else if (option != "q"){
                cout << "Invalid choice; please try again." << endl;
            }
        }

       loadAnother = getYesOrNo("Load another file? (y/n) ", "Please type a word that starts with 'Y' or 'N'.");
    }

    cout << "Have a nice Life!" << endl;
    LifeGUI::shutdown();
    return 0;
}



void readFileToGrid(Grid<string>& g){
    ifstream infile;
    string fileName = "";
    Vector<string> inputContent;

    infile.open(fileName);
    promptUserForFile(infile, "Grid input file name? ", "Unable to open that file. Try again.");

    string line;
    while (getline(infile, line)) {
        inputContent.add(line);
    }
    infile.close();

    int rows = stringToInteger(inputContent[0]);
    int cols = stringToInteger(inputContent[1]);

    g.resize(rows, cols);
    // This block of code is designed to weed out garbage information.
    // Note that since the input file has two lines above the grid input, row index need to be subtrated 2 to be matched.
    for (int i = 2; i < rows + 2; i++){
        for (int j = 0; j < cols; j++){
            g[i-2][j] = inputContent[i][j];
        }
    }
}

/*
 * Prints the current status of a grid in correct format onto console.
 * The function accepts a constant reference to a grid to obtain grid
 * information without making changes.
 */
void displayGrid(const Grid<string>& g, int rows, int cols){

    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            cout << g[i][j];
        }
        cout << endl;
    }
}

/*
 * Determines what a cell's status would be in the next generation.
 * The function accepts a reference to a grid, a row index, and a
 * column index, finds out the number of living neighbors of cell
 * located at this row index and column index.
 * Returns "-"(dead) if number of living neighbors <=1 or >=4,
 * returns original status if number of living neighbors = 2,
 * returns "X"(alive) if number of living neighbors = 3.
 */
string tickSingleCell(Grid<string>& g, int rowInd, int colInd, int rows, int cols){

    string neighbors = "";

    // This block of code would get the index of given cell's neighbors
    // If a neighbor is out of bounds, modulus operator would take the farest possible index instead.
    // (+ cols) is to make sure there is no negative index (since negative int % int = negative int).
    int leftInd = (colInd - 1 + cols) % cols;
    int rightInd = (colInd + 1 + cols) % cols;
    int upInd = (rowInd - 1 + rows) % rows;
    int downInd = (rowInd + 1 + rows) % rows;

    // Here the status of eight cells next to this cell is stored in a string, which is later looped
    // through to count number of living cells.
    neighbors = neighbors + g[upInd][leftInd] + g[upInd][colInd] + g[upInd][rightInd] + g[rowInd][leftInd]
            + g[rowInd][rightInd] + g[downInd][leftInd] + g[downInd][colInd] + g[downInd][rightInd];

    int livingNeighbors = 0;
    for (int i = 0; i < neighbors.length(); i++){
        if (neighbors[i] == 'X'){
            livingNeighbors += 1;
        }
    }

    if (livingNeighbors <= 1 || livingNeighbors >= 4){
        return "-";
    } else if (livingNeighbors == 3){
        return "X";
    } else{
        return g[rowInd][colInd];
    }
}

/*
 * Updates a given gird to its next generation.
 * The function accepts a reference to a grid, and updates every
 * cell in it.
 */
void tickGrid(Grid<string>& g, int rows, int cols){

    // Create a new grid and store the updated cells in the new grid, so that the updated
    // results would not affect our calculation. Eventually, set new grid equal to old grid.
    Grid<string> nextGeneration(rows, cols);

    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            nextGeneration[i][j] = tickSingleCell(g, i, j, rows, cols);
        }
    }
    g = nextGeneration;
}


/*
 * Updates the current status of a grid in Graphical User Interface.
 * The function accepts a constant reference to a grid to obtain grid
 * information without making changes.
 */
void updateGUI(const Grid<string>& g, int rows, int cols){
    LifeGUI::clear();
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            if (g[i][j] == "X"){
                LifeGUI::fillCell(i, j);
            }
        }
    }
    LifeGUI::repaint();
}
