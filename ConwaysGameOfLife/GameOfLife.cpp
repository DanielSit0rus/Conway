using namespace std;



#include <iostream>
#include <vector>
#include <unordered_map>
#include <cstdlib>  // For srand and rand
#include <ctime>    // For time
#include <fstream>  // For file handling


class Cell {
private:
    bool alive;

public:
    void setAlive() {
        alive = true;
    }
    void setDead() {
        alive = false;
    }
    void switchState() {
        if (alive) {
            setDead();
        }
        else {
            setAlive();
        }
    }
    bool isAlive() const {
        return alive;
    }

};

template<typename T>
class Grid {
private:
    std::vector < std::vector <T>> grid;
    int rows;
    int columns;

public:
    Grid(int rows, int columns); 
    ~Grid() {
    }
    int getRows() const;
    int getColumns() const;
    void initializeGrid(int numAliveCells); 
    void updateGrid();
    T& getCell(int row, int col);
    bool getCellState(int row, int col) const;
    int countAliveNeighbors(int row, int col) const;



    bool operator==(const Grid<T>& other) const;


    // Friend function to overload the << operator for printing the grid
    template<typename T>
    friend std::ostream& operator<<(std::ostream& os, const Grid& grid);
};

template<typename T>
Grid<T>::Grid(int rows, int columns) {
    this->rows = rows;
    this->columns = columns;
    grid.resize(rows, vector<Cell>(columns));
}

template<typename T>
T& Grid<T>::getCell(int row, int col) {
    return grid[row][col];
}



// Destructor
template<typename T>
void Grid<T>::initializeGrid(int numAliveCells) {

    std::srand(static_cast<unsigned int>(std::time(0)));//seed based on time
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            grid[i][j].setDead();

        }
    }

    int aliveCount = 0;
    while (aliveCount < numAliveCells) {
        int randomRow = std::rand() % rows; //check this
        int randomCol = std::rand() % columns;

        if (!grid[randomRow][randomCol].isAlive()) {
            grid[randomRow][randomCol].setAlive();
            aliveCount++;

        }
    }

}

template<typename T>
bool Grid<T>::operator==(const Grid<T>& other) const {
    if (rows != other.rows || columns != other.columns) {
        return false; // Grids have same dimensions.
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            if (grid[i][j].isAlive() != other.grid[i][j].isAlive()) {
                return false; // if any cell differs, grids are not equal
            }
        }
    }
    return true;
}


// Overload the << operator for printing the grid
template<typename T>
std::ostream& operator<<(std::ostream& os, const Grid<T>& grid) {
    int displayRows = grid.getRows() * 2 + 1;
    int displayColumns = grid.getColumns() * 2 + 1;

    for (int i = 0; i < displayRows; i++) {
        for (int j = 0; j < displayColumns; j++) {
            if (i % 2 == 0 && j % 2 == 0) {
                os << ".";
            }
            else if (i % 2 == 1 && j % 2 == 1) {
                if (grid.getCellState(i / 2, j / 2)) {
                    os << "O";  // Alive cell
                }
                else {
                    os << " ";  // Dead cell
                }
            }
            else {
                os << " ";  // Space between dots and cells
            }
        }
        os << std::endl;
    }
    return os;
}

template<typename T>
int Grid<T>::countAliveNeighbors(int row, int col) const {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            int rowNeighbor = row + i;
            int columnNeighbor = col + j;

            if (rowNeighbor >= 0 && rowNeighbor < rows && columnNeighbor >= 0 && columnNeighbor < columns) {
                if (grid[rowNeighbor][columnNeighbor].isAlive()) {
                    count++;
                }
            }

        }
    }
    return count;

}

template<typename T>
void Grid<T>::updateGrid() {
    vector<vector<Cell>> newGrid = grid;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            int aliveNeighbors = countAliveNeighbors(i, j);

            if (grid[i][j].isAlive()) {
                // live if fewer than 2 or more than 3 neighbor
                if (aliveNeighbors < 2 || aliveNeighbors > 3) {
                    newGrid[i][j].setDead();
                }
            }
            else {
                // alive
                if (aliveNeighbors == 3) {
                    newGrid[i][j].setAlive();
                }
            }
        }
    }
    grid = newGrid;
}

template<typename T>
bool Grid<T>::getCellState(int row, int col) const {
    return grid[row][col].isAlive();
}

template<typename T>
int Grid<T>::getRows() const {
    return this->rows;

}
template<typename T>
int Grid<T>::getColumns() const {
    return this->columns;

}

template<typename T>
class Pattern {
protected:
    std::string patternName;

public:
    Pattern(const std::string& patternName) {
        this->patternName = patternName;
    }

    virtual bool detectPattern(const Grid<T>& grid) const = 0;

    string getName() const {
        return this->patternName;
    }

    virtual ~Pattern() {}

};

template<typename T>
class BlockPattern : public Pattern<T> {
public:
    BlockPattern() : Pattern<T>("Block") {}

    bool detectPattern(const Grid<T>& grid) const override {
        for (int i = 0; i < grid.getRows() - 1; i++) {
            for (int j = 0; j < grid.getColumns() - 1; j++) {
                if (grid.getCellState(i, j) && grid.getCellState(i, j + 1) &&
                    grid.getCellState(i + 1, j) && grid.getCellState(i + 1, j + 1)) {
                    if (grid.countAliveNeighbors(i, j) == 3 &&
                        grid.countAliveNeighbors(i, j + 1) == 3 &&
                        grid.countAliveNeighbors(i + 1, j) == 3 &&
                        grid.countAliveNeighbors(i + 1, j + 1) == 3) {
                        return true; // block found
                    }


                }
            }
        }
        return false; // block not found
    }


};
template<typename T>
class BeehivePattern : public Pattern<T> {
public:
    BeehivePattern() : Pattern<T>("Beehive") {}

    bool detectPattern(const Grid<T>& grid) const override {
        for (int i = 0; i < grid.getRows() - 3; i++) {
            for (int j = 0; j < grid.getColumns() - 5; j++) {
                // Check Beehive shape:
                if (grid.getCellState(i + 1, j + 2) && grid.getCellState(i + 1, j + 3) &&  // Top row
                    grid.getCellState(i + 2, j + 1) && grid.getCellState(i + 2, j + 4) &&  // Middle row
                    grid.getCellState(i + 3, j + 2) && grid.getCellState(i + 3, j + 3)) {  // Bottom row

                    // Now use countAliveNeighbors to ensure that each Beehive cell has exactly 2 neighbors
                    if (grid.countAliveNeighbors(i + 1, j + 2) == 2 &&  // Top-left cell
                        grid.countAliveNeighbors(i + 1, j + 3) == 2 &&  // Top-right cell
                        grid.countAliveNeighbors(i + 2, j + 1) == 2 &&  // Middle-left cell
                        grid.countAliveNeighbors(i + 2, j + 4) == 2 &&  // Middle-right cell
                        grid.countAliveNeighbors(i + 3, j + 2) == 2 &&  // Bottom-left cell
                        grid.countAliveNeighbors(i + 3, j + 3) == 2) {  // Bottom-right cell
                        return true;  // Valid Beehive found
                    }
                }
            }
        }
        return false;  // No Beehive pattern found
    }
};

template<typename T>
class BlinkerPattern : public Pattern<T> {
public:
    BlinkerPattern() : Pattern<T>("Blinker") {}

    bool detectPattern(const Grid<T>& grid) const override {
        for (int i = 0; i < grid.getRows(); i++) {
            for (int j = 0; j < grid.getColumns() - 2; j++) {
                if (doesHorizontalBlinkerExist(grid, i, j)) {
                    std::cout << "Horizontal blinker detected at (" << i << "," << j << ")" << std::endl;

                    // Simulate two steps and check if the pattern returns to its original configuration
                    Grid<T> nextGrid = grid;
                    nextGrid.updateGrid();
                    nextGrid.updateGrid();

                    if (doesHorizontalBlinkerExist(nextGrid, i, j)) {
                        return true;
                    }
                }
            }
        }

        for (int i = 0; i < grid.getRows() - 2; i++) {
            for (int j = 0; j < grid.getColumns(); j++) {

                if (doesVerticalBlinkerExist(grid, i, j)) {
                    std::cout << "Vertical blinker detected at (" << i << "," << j << ")" << std::endl;

                    Grid<T> nextGrid = grid;
                    nextGrid.updateGrid();  // First update
                    nextGrid.updateGrid();  // Second update


                    if (doesVerticalBlinkerExist(nextGrid, i, j)) {
                        return true;  // Full oscillation cycle detected
                    }
                }
            }
        }

        return false;  // No blinker pattern found
    }

private:
    // Detect horizontal blinker
    bool doesHorizontalBlinkerExist(const Grid<T>& grid, int row, int col) const {
        if (col + 2 >= grid.getColumns()) return false;
        return
            grid.getCellState(row, col) &&
            grid.getCellState(row, col + 1) &&
            grid.getCellState(row, col + 2) &&
            grid.countAliveNeighbors(row, col) == 1 &&
            grid.countAliveNeighbors(row, col + 1) == 2 &&
            grid.countAliveNeighbors(row, col + 2) == 1;
    }

    // Detect vertical blinker
    bool doesVerticalBlinkerExist(const Grid<T>& grid, int row, int col) const {
        if (row + 2 >= grid.getRows()) return false;
        return
            grid.getCellState(row, col) &&
            grid.getCellState(row + 1, col) &&
            grid.getCellState(row + 2, col) &&
            grid.countAliveNeighbors(row, col) == 1 &&
            grid.countAliveNeighbors(row + 1, col) == 2 &&
            grid.countAliveNeighbors(row + 2, col) == 1;
    }
};
;

template<typename T>
class ToadPattern : public Pattern<T> {
public:
    ToadPattern() : Pattern<T>("Toad") {}

    bool detectPattern(const Grid<T>& grid) const override {
        for (int i = 0; i < grid.getRows() - 2; i++) {
            for (int j = 0; j < grid.getColumns() - 4; j++) {
                // Check for the first configuration of Toad (S shape)
                if (doesToadExist(grid, i, j)) {
                    std::cout << "First Toad config detected at (" << i << "," << j << ")" << std::endl;

                    // Simulate two steps and check if the pattern returns to its original configuration
                    Grid<T> nextGrid = grid;
                    nextGrid.updateGrid();  // First update
                    std::cout << "Grid after first update:\n" << nextGrid << std::endl;

                    nextGrid.updateGrid();  // Second update
                    std::cout << "Grid after second update:\n" << nextGrid << std::endl;

                    if (doesToadExist(nextGrid, i, j)) {
                        return true;  // Full cycle detected
                    }
                }
            }
        }
        return false;  // No Toad pattern found
    }

private:
    // Check if the first Toad configuration (staggered "S" shape) exists at the given position
    bool doesToadExist(const Grid<T>& grid, int row, int col) const {
        return doesHorizontalToadExist(grid, row, col) || doesFlippedHorizontalToadExist(grid, row, col) ||
            doesVerticalToadExist(grid, row, col) || doesFlippedVerticalToadExist(grid, row, col);
    }

    bool doesHorizontalToadExist(const Grid<T>& grid, int row, int col) const {
        if (row + 1 >= grid.getRows() || col + 3 >= grid.getColumns()) return false;
        return
            grid.getCellState(row, col + 1) &&
            grid.getCellState(row, col + 2) &&
            grid.getCellState(row, col + 3) &&
            grid.getCellState(row + 1, col) &&
            grid.getCellState(row + 1, col + 1) &&
            grid.getCellState(row + 1, col + 2) &&

            grid.countAliveNeighbors(row, col + 1) == 4 &&
            grid.countAliveNeighbors(row, col + 2) == 4 &&
            grid.countAliveNeighbors(row, col + 3) == 2 &&
            grid.countAliveNeighbors(row + 1, col) == 2 &&
            grid.countAliveNeighbors(row + 1, col + 1) == 4 &&
            grid.countAliveNeighbors(row + 1, col + 2) == 4;
    }


    bool doesFlippedHorizontalToadExist(const Grid<T>& grid, int row, int col) const {
        if (row + 1 >= grid.getRows() || col + 3 >= grid.getColumns()) return false;
        return
            grid.getCellState(row, col) &&
            grid.getCellState(row, col + 1) &&
            grid.getCellState(row, col + 2) &&
            grid.getCellState(row + 1, col + 1) &&
            grid.getCellState(row + 1, col + 2) &&
            grid.getCellState(row + 1, col + 3) &&

            grid.countAliveNeighbors(row, col) == 2 &&
            grid.countAliveNeighbors(row, col + 1) == 4 &&
            grid.countAliveNeighbors(row, col + 2) == 4 &&
            grid.countAliveNeighbors(row + 1, col + 1) == 4 &&
            grid.countAliveNeighbors(row + 1, col + 2) == 4 &&
            grid.countAliveNeighbors(row + 1, col + 3) == 2;
    }

    bool doesVerticalToadExist(const Grid<T>& grid, int row, int col) const {
        if (row + 3 >= grid.getRows() || col + 1 >= grid.getColumns()) return false;
        return
            grid.getCellState(row + 1, col) &&
            grid.getCellState(row + 2, col) &&
            grid.getCellState(row + 3, col) &&
            grid.getCellState(row, col + 1) &&
            grid.getCellState(row + 1, col + 1) &&
            grid.getCellState(row + 2, col + 1) &&

            grid.countAliveNeighbors(row + 1, col) == 4 &&
            grid.countAliveNeighbors(row + 2, col) == 4 &&
            grid.countAliveNeighbors(row + 3, col) == 2 &&
            grid.countAliveNeighbors(row, col + 1) == 2 &&
            grid.countAliveNeighbors(row + 1, col + 1) == 4 &&
            grid.countAliveNeighbors(row + 2, col + 1) == 4;
    }

    bool doesFlippedVerticalToadExist(const Grid<T>& grid, int row, int col) const {
        if (row + 3 >= grid.getRows() || col + 1 >= grid.getColumns()) return false;
        return
            grid.getCellState(row, col) &&
            grid.getCellState(row + 1, col) &&
            grid.getCellState(row + 2, col) &&
            grid.getCellState(row + 1, col + 1) &&
            grid.getCellState(row + 2, col + 1) &&
            grid.getCellState(row + 3, col + 1) &&

            grid.countAliveNeighbors(row, col) == 2 &&
            grid.countAliveNeighbors(row + 1, col) == 4 &&
            grid.countAliveNeighbors(row + 2, col) == 4 &&
            grid.countAliveNeighbors(row + 1, col + 1) == 4 &&
            grid.countAliveNeighbors(row + 2, col + 1) == 4 &&
            grid.countAliveNeighbors(row + 3, col + 1) == 2;
    }
};


template<typename T>
class GliderPattern : public Pattern<T> {
public:
    GliderPattern() : Pattern<T>("Glider") {}

    bool detectPattern(const Grid<T>& grid) const override {
        for (int i = 0; i < grid.getRows() - 2; i++) {
            for (int j = 0; j < grid.getColumns() - 2; j++) {
                // Check for the glider's first configuration
                if (doesGliderExist(grid, i, j)) {

                    Grid<T> nextGrid = grid;
                    for (int step = 0; step < 4; ++step) {
                        nextGrid.updateGrid();
                    }
                    if (doesGlider1Exist(nextGrid, i + 1, j + 1)
                        || doesGlider2Exist(nextGrid, i - 1, j + 1)
                        || doesGlider3Exist(nextGrid, i + 1, j - 1)
                        || doesGlider4Exist(nextGrid, i - 1, j - 1)) {
                        return true; // Full cycle and diagonal shift detected
                    }

                }
            }
        }
        return false;
    }

private:
    bool doesGliderExist(const Grid<T>& grid, int row, int col) const {
        return doesGlider1Exist(grid, row, col) || doesGlider2Exist(grid, row, col) || doesGlider3Exist(grid, row, col) || doesGlider4Exist(grid, row, col);
    }
    // Check if the given position contains the glider's starting configuration
    bool doesGlider1Exist(const Grid<T>& grid, int row, int col) const {
        if (row + 2 >= grid.getRows() || col + 2 >= grid.getColumns()) {
            return false;
        }
        return grid.getCellState(row + 1, col) &&      
            grid.getCellState(row + 2, col + 1) &&   
            grid.getCellState(row, col + 2) &&       
            grid.getCellState(row + 1, col + 2) &&   
            grid.getCellState(row + 2, col + 2) &&    

            grid.countAliveNeighbors(row + 1, col) == 1 &&
            grid.countAliveNeighbors(row + 2, col + 1) == 3 &&
            grid.countAliveNeighbors(row, col + 2) == 1 &&
            grid.countAliveNeighbors(row + 1, col + 2) == 3 &&
            grid.countAliveNeighbors(row + 2, col + 2) == 2;
    }
    bool doesGlider2Exist(const Grid<T>& grid, int row, int col) const {

        if (row + 2 >= grid.getRows() || col + 2 >= grid.getColumns()) {
            return false;
        }

        return
            grid.getCellState(row + 1, col) &&
            grid.getCellState(row, col + 1) &&
            grid.getCellState(row, col + 2) &&
            grid.getCellState(row + 1, col + 2) &&
            grid.getCellState(row + 2, col + 2) &&

            // Ensure that neighbors match the glider's requirement (isolated)
            grid.countAliveNeighbors(row + 1, col) == 1 &&
            grid.countAliveNeighbors(row, col + 1) == 3 &&
            grid.countAliveNeighbors(row, col + 2) == 2 &&
            grid.countAliveNeighbors(row + 1, col + 2) == 3 &&
            grid.countAliveNeighbors(row + 2, col + 2) == 1;
    }

    bool doesGlider3Exist(const Grid<T>& grid, int row, int col) const {

        // Check if row + 2 and col + 1 are within bounds
        if (row + 2 >= grid.getRows() || col + 2 >= grid.getColumns()) {
            cout << "HII";
            return false;
        }

        return
            grid.getCellState(row, col) &&          
            grid.getCellState(row + 1, col) &&      
            grid.getCellState(row + 2, col) &&      
            grid.getCellState(row + 2, col + 1) && 
            grid.getCellState(row + 1, col + 2) && 

            // Ensure the neighbor counts match the expected glider configuration
            grid.countAliveNeighbors(row, col) == 1 &&
            grid.countAliveNeighbors(row + 1, col) == 3 &&
            grid.countAliveNeighbors(row + 2, col) == 2 &&
            grid.countAliveNeighbors(row + 2, col + 1) == 3 &&
            grid.countAliveNeighbors(row + 1, col + 2) == 1;
    }
    bool doesGlider4Exist(const Grid<T>& grid, int row, int col) const {

        if (row + 2 >= grid.getRows() || col + 2 >= grid.getColumns()) {
            return false;
        }

        return
            grid.getCellState(row, col) &&          
            grid.getCellState(row + 1, col) &&      
            grid.getCellState(row + 2, col) &&      
            grid.getCellState(row + 1, col) &&  
            grid.getCellState(row + 1, col + 2) &&  

            // Ensure the neighbor counts match the expected glider configuration
            grid.countAliveNeighbors(row, col) == 2 &&
            grid.countAliveNeighbors(row + 1, col) == 3 &&
            grid.countAliveNeighbors(row + 2, col) == 1 &&
            grid.countAliveNeighbors(row + 1, col) == 3 &&
            grid.countAliveNeighbors(row + 1, col + 2) == 1;
    }

};

template<typename T>
class LWSSPattern : public Pattern<T> {
public:
    LWSSPattern() : Pattern<T>("Light Weight Space Ship") {}

    bool detectPattern(const Grid<T>& grid) const override {
        for (int i = 0; i < grid.getRows() - 4; i++) {
            for (int j = 0; j < grid.getColumns() - 5; j++) {
                // Check for the LWSS configuration
                if (doesLWSS1Exist(grid, i, j)) {
                    //Grid<T> nextGrid = grid;
                    //for (int step = 0; step < 4; ++step) {
                    //    nextGrid.updateGrid();
                    //}
                    //if (doesLWSS1Exist(nextGrid, i, j - 2)
                    //    || doesLWSS2Exist(nextGrid, i, j + 2)
                    //    || doesLWSS3Exist(nextGrid, i, j + 2)
                    //    || doesLWSS4Exist(nextGrid, i, j - 2)) {
                    //    return true; // Full cycle and horizontal shift detected
                    //}
                    //

                    return true;
                }
            }
        }
        return false;
    }

private:
    // Check if the LWSS pattern exists at the given location
    bool doesLWSS1Exist(const Grid<T>& grid, int row, int col) const {
        if (row + 3 >= grid.getRows() || col + 4 >= grid.getColumns()) {
            std::cout << "Out of bounds at (" << row << "," << col << ")" << std::endl;
            return false;
        }

        return
            grid.getCellState(row, col + 1) &&
            grid.getCellState(row, col + 4) &&
            grid.getCellState(row + 1, col) &&
            grid.getCellState(row + 2, col) &&
            grid.getCellState(row + 2, col + 4) &&
            grid.getCellState(row + 3, col) &&
            grid.getCellState(row + 3, col + 1) &&
            grid.getCellState(row + 3, col + 2) &&
            grid.getCellState(row + 3, col + 3) &&


            grid.countAliveNeighbors(row, col + 1) == 1 &&
            grid.countAliveNeighbors(row, col + 4) == 0 &&
            grid.countAliveNeighbors(row + 1, col) == 2 &&
            grid.countAliveNeighbors(row + 2, col) == 3 &&
            grid.countAliveNeighbors(row + 2, col + 4) == 1 &&
            grid.countAliveNeighbors(row + 3, col) == 2 &&
            grid.countAliveNeighbors(row + 3, col + 1) == 3 &&
            grid.countAliveNeighbors(row + 3, col + 2) == 2 &&
            grid.countAliveNeighbors(row + 3, col + 3) == 2;

    }

    bool doesLWSS2Exist(const Grid<T>& grid, int row, int col) const {
        if (row + 3 >= grid.getRows() || col + 4 >= grid.getColumns()) {
            return false;
        }

        return
            grid.getCellState(row, col) &&
            grid.getCellState(row, col + 3) &&
            grid.getCellState(row + 1, col + 4) &&
            grid.getCellState(row + 2, col) &&
            grid.getCellState(row + 2, col + 4) &&
            grid.getCellState(row + 3, col + 1) &&
            grid.getCellState(row + 3, col + 2) &&
            grid.getCellState(row + 3, col + 3) &&
            grid.getCellState(row + 3, col + 4) &&

            grid.countAliveNeighbors(row, col) == 0 &&
            grid.countAliveNeighbors(row, col + 3) == 1 &&
            grid.countAliveNeighbors(row + 1, col + 4) == 2 &&
            grid.countAliveNeighbors(row + 2, col) == 1 &&
            grid.countAliveNeighbors(row + 2, col + 4) == 3 &&
            grid.countAliveNeighbors(row + 3, col + 1) == 2 &&
            grid.countAliveNeighbors(row + 3, col + 2) == 2 &&
            grid.countAliveNeighbors(row + 3, col + 3) == 3 &&
            grid.countAliveNeighbors(row + 3, col + 4) == 2;
    }

    bool doesLWSS3Exist(const Grid<T>& grid, int row, int col) const {
        if (row + 3 >= grid.getRows() || col + 4 >= grid.getColumns()) {
            return false;
        }

        return

            grid.getCellState(row, col + 1) &&
            grid.getCellState(row, col + 2) &&
            grid.getCellState(row, col + 3) &&
            grid.getCellState(row, col + 4) &&
            grid.getCellState(row + 1, col) &&
            grid.getCellState(row + 1, col + 4) &&
            grid.getCellState(row + 2, col + 4) &&
            grid.getCellState(row + 3, col) &&
            grid.getCellState(row + 3, col + 3) &&


            grid.countAliveNeighbors(row, col + 1) == 2 &&
            grid.countAliveNeighbors(row, col + 2) == 2 &&
            grid.countAliveNeighbors(row, col + 3) == 3 &&
            grid.countAliveNeighbors(row, col + 4) == 2 &&
            grid.countAliveNeighbors(row + 1, col) == 1 &&
            grid.countAliveNeighbors(row + 1, col + 4) == 3 &&
            grid.countAliveNeighbors(row + 2, col + 4) == 2 &&
            grid.countAliveNeighbors(row + 3, col) == 0 &&
            grid.countAliveNeighbors(row + 3, col + 3) == 1;
    }

    bool doesLWSS4Exist(const Grid<T>& grid, int row, int col) const {
        if (row + 3 >= grid.getRows() || col + 4 >= grid.getColumns()) {
            return false;
        }

        return
            grid.getCellState(row, col) &&
            grid.getCellState(row, col + 1) &&
            grid.getCellState(row, col + 2) &&
            grid.getCellState(row, col + 3) &&

            grid.getCellState(row + 1, col) &&
            grid.getCellState(row + 1, col + 4) &&

            grid.getCellState(row + 2, col) &&

            grid.getCellState(row + 3, col + 1) &&
            grid.getCellState(row + 3, col + 4) &&

            grid.countAliveNeighbors(row, col) == 2 &&
            grid.countAliveNeighbors(row, col + 1) == 3 &&
            grid.countAliveNeighbors(row, col + 2) == 2 &&
            grid.countAliveNeighbors(row, col + 3) == 2 &&
            grid.countAliveNeighbors(row + 1, col) == 3 &&
            grid.countAliveNeighbors(row + 1, col + 4) == 1 &&
            grid.countAliveNeighbors(row + 2, col) == 2 &&
            grid.countAliveNeighbors(row + 3, col + 1) == 1 &&
            grid.countAliveNeighbors(row + 3, col + 4) == 0;
    }

    bool doesLWSS5Exist(const Grid<T>& grid, int row, int col) const {
        if (row + 3 >= grid.getRows() || col + 4 >= grid.getColumns()) {
            return false;
        }

        return
            grid.getCellState(row, col) &&
            grid.getCellState(row, col + 2) &&


            grid.getCellState(row + 1, col + 3) &&


            grid.getCellState(row + 2, col + 3) &&


            grid.getCellState(row + 3, col) &&
            grid.getCellState(row + 3, col + 3) &&


            grid.getCellState(row + 4, col + 1) &&
            grid.getCellState(row + 4, col + 2) &&
            grid.getCellState(row + 4, col + 3) &&


            grid.countAliveNeighbors(row, col) == 0 &&
            grid.countAliveNeighbors(row, col + 2) == 1 &&
            grid.countAliveNeighbors(row + 1, col + 3) == 2 &&
            grid.countAliveNeighbors(row + 2, col + 3) == 2 &&
            grid.countAliveNeighbors(row + 3, col) == 1 &&
            grid.countAliveNeighbors(row + 3, col + 3) == 3 &&
            grid.countAliveNeighbors(row + 4, col + 1) == 2 &&
            grid.countAliveNeighbors(row + 4, col + 2) == 3 &&
            grid.countAliveNeighbors(row + 4, col + 3) == 2;
    }

    bool doesLWSS6Exist(const Grid<T>& grid, int row, int col) const {
        if (row + 3 >= grid.getRows() || col + 4 >= grid.getColumns()) {
            return false;
        }

        return

            grid.getCellState(row, col + 1) &&
            grid.getCellState(row, col + 3) &&

            grid.getCellState(row + 1, col) &&

            grid.getCellState(row + 2, col) &&

            grid.getCellState(row + 3, col) &&
            grid.getCellState(row + 3, col + 3) &&

            grid.getCellState(row + 4, col) &&
            grid.getCellState(row + 4, col + 1) &&
            grid.getCellState(row + 4, col + 2) &&


            grid.countAliveNeighbors(row, col + 1) == 1 &&
            grid.countAliveNeighbors(row, col + 3) == 0 &&
            grid.countAliveNeighbors(row + 1, col) == 2 &&
            grid.countAliveNeighbors(row + 2, col) == 2 &&
            grid.countAliveNeighbors(row + 3, col) == 3 &&
            grid.countAliveNeighbors(row + 3, col + 3) == 1 &&
            grid.countAliveNeighbors(row + 4, col) == 2 &&
            grid.countAliveNeighbors(row + 4, col + 1) == 3 &&
            grid.countAliveNeighbors(row + 4, col + 2) == 2;
    }

    bool doesLWSS7Exist(const Grid<T>& grid, int row, int col) const {
        if (row + 3 >= grid.getRows() || col + 4 >= grid.getColumns()) {
            return false;
        }

        return

            grid.getCellState(row, col) &&
            grid.getCellState(row, col + 1) &&
            grid.getCellState(row, col + 2) &&

            grid.getCellState(row + 1, col) &&
            grid.getCellState(row + 1, col + 3) &&


            grid.getCellState(row + 2, col) &&


            grid.getCellState(row + 3, col) &&

            grid.getCellState(row + 4, col + 1) &&
            grid.getCellState(row + 4, col + 3) &&


            grid.countAliveNeighbors(row, col) == 2 &&
            grid.countAliveNeighbors(row, col + 1) == 3 &&
            grid.countAliveNeighbors(row, col + 2) == 2 &&
            grid.countAliveNeighbors(row + 1, col) == 3 &&
            grid.countAliveNeighbors(row + 1, col + 3) == 1 &&
            grid.countAliveNeighbors(row + 2, col) == 2 &&
            grid.countAliveNeighbors(row + 3, col) == 2 &&
            grid.countAliveNeighbors(row + 4, col + 1) == 1 &&
            grid.countAliveNeighbors(row + 4, col + 3) == 0;
    }

    bool doesLWSS8Exist(const Grid<T>& grid, int row, int col) const {
        if (row + 3 >= grid.getRows() || col + 4 >= grid.getColumns()) {
            return false;
        }

        return
            grid.getCellState(row, col + 1) &&
            grid.getCellState(row, col + 2) &&
            grid.getCellState(row, col + 3) &&

            grid.getCellState(row + 1, col) &&
            grid.getCellState(row + 1, col + 3) &&


            grid.getCellState(row + 2, col + 3) &&

            grid.getCellState(row + 3, col + 3) &&

            grid.getCellState(row + 4, col) &&
            grid.getCellState(row + 4, col + 2) &&


            grid.countAliveNeighbors(row, col + 1) == 2 &&
            grid.countAliveNeighbors(row, col + 2) == 3 &&
            grid.countAliveNeighbors(row, col + 3) == 2 &&
            grid.countAliveNeighbors(row + 1, col) == 1 &&
            grid.countAliveNeighbors(row + 1, col + 3) == 3 &&
            grid.countAliveNeighbors(row + 2, col + 3) == 2 &&
            grid.countAliveNeighbors(row + 3, col + 3) == 2 &&
            grid.countAliveNeighbors(row + 4, col) == 0 &&
            grid.countAliveNeighbors(row + 4, col + 2) == 1;


    }

};


class Game {
private:
    Grid<Cell>* grid;
    Grid<Cell>* initialGrid;
    int rows;
    int columns;
    int numAliveCells;
    int steps;
    int stepsCount;

public:
    Game() : grid(nullptr), initialGrid(nullptr), rows(0), columns(0), numAliveCells(0), steps(0), stepsCount(0) {}

    ~Game() {
        delete grid;  // Clean up dynamically allocated memory
        delete initialGrid;
    }

    void initializeLWSS1Pattern() {
        // Ensure the grid is cleaned before placing the LWSS1 pattern
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < columns; ++j) {
                grid->getCell(i, j).setDead();
            }
        }

        // Place the LWSS1 pattern (relative to the top-left of the grid)
        if (rows >= 5 && columns >= 4) {
            // First row
            grid->getCell(0, 1).setAlive();  // 0,1 (alive)
            grid->getCell(0, 4).setAlive();  // 0,4 (alive)

            // Second row
            grid->getCell(1, 0).setAlive();  // 1,0 (alive)

            // Third row
            grid->getCell(2, 0).setAlive();  // 2,0 (alive)
            grid->getCell(2, 4).setAlive();  // 2,4 (alive)

            // Fourth row
            grid->getCell(3, 0).setAlive();  // 3,0 (alive)
            grid->getCell(3, 1).setAlive();  // 3,1 (alive)
            grid->getCell(3, 2).setAlive();  // 3,2 (alive)
            grid->getCell(3, 3).setAlive();  // 3,3 (alive)
        }
        else {
            std::cout << "Grid is too small for LWSS7 pattern!" << std::endl;
        }
    }





    void start() {

        cout << "How many rows would you like?" << endl;
        cin >> this->rows;
        cout << "How many columns would you like?" << endl;
        cin >> this->columns;
        cout << "How many alive cells would you want to start with?" << endl;
        cin >> this->numAliveCells;
        cout << "Enter the number of steps to stimulate: ";
        cin >> this->steps;

        delete grid;
        delete initialGrid;

        grid = new Grid<Cell>(rows, columns);
        grid->initializeGrid(numAliveCells);

        initialGrid = new Grid<Cell>(rows, columns);
        *initialGrid = *grid;

        stepsCount = 0; // reset when starting new game
    }

    void restartGame() {
        if (initialGrid != nullptr) {
            // Reset the current grid to the saved initial grid
            *grid = *initialGrid;
            stepsCount = 0;  // Reset the steps count
            cout << "Game has been restarted with the initial configuration." << endl;
        }
        else {
            cout << "No game to restart." << endl;
        }
    }


    void simulate() {
        string continueOption = "y";

        cout << "\033[H\033[J";  // Clear the screen and move the cursor to the top-left corner
        cout << *grid;

        while (continueOption == "y" && stepsCount < steps) {

            cout << "Would you like to continue next step or save the game? (y/n). To reset press R: ";
            cin >> continueOption;

            while (continueOption != "y" && continueOption != "n" && continueOption != "s" && continueOption != "r") {
                cout << "Invalid input. Please enter 'y' to continue, 's' to save, or 'n' to stop: ";
                cin >> continueOption;
            }

            if (continueOption == "s") {
                string filename;
                cout << "Enter the filename to save the game: ";
                cin >> filename;
                saveGame(filename);
                cout << "Game has been saved. Would you like to continue? (y/n): ";
                cin >> continueOption;

                while (continueOption != "y" && continueOption != "n") {
                    cout << "Invalid input. Please enter 'y' or 'n': ";
                    cin >> continueOption;
                }

                if (continueOption == "n") {
                    break;
                }
            }

            if (continueOption == "y") {
                grid->updateGrid();
                stepsCount++;

                cout << "\033[H";  // Move cursor back to the top-left corner without clearing the screen
                cout << *grid;  // Reprint the grid in place

                if (stepsCount >= steps) {
                    cout << "Maximum steps have been reached. Simulation ended." << endl;
                    break;
                }
            }

            if (continueOption == "r") {
                restartGame();  // Call the restartGame method
                simulate();
                return;
            }
        }

        if (continueOption == "n") {
            cout << "Simulation stopped by user." << endl;
        }

        cout << *grid;

        delete grid;
        grid = nullptr;
    }



    void loadGame(const string& file) {
        ifstream inFile(file);
        if (!inFile) {
            cerr << "Error" << endl;
            return;
        }
        inFile >> rows >> columns >> stepsCount >> steps; // learn this (read grid dimensions, steps count and total steps)

        delete grid;
        grid = new Grid<Cell>(rows, columns);
        int cellStateInt;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                inFile >> cellStateInt;  // Read an int, 1 for alive, 0 for dead
                if (cellStateInt == 1) {
                    grid->getCell(i, j).setAlive();  // Set cell alive
                }
                else {
                    grid->getCell(i, j).setDead();   // Set cell dead
                }
            }
        }
        inFile.close();

        if (initialGrid) {
            delete initialGrid;
        }
        initialGrid = new Grid<Cell>(*grid);

        cout << "Game has been loaded from " << file << endl;

    }

    void saveGame(string file) {

        ofstream outFile(file);
        if (!outFile) {
            cerr << "Error" << endl;
            return;
        }
        outFile << rows << " " << columns << " " << stepsCount << " " << steps << endl;
        // Save the grid state (alive or dead for each cell)
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < columns; ++j) {
                outFile << (grid->getCellState(i, j) ? 1 : 0) << " ";  // Save 1 for alive, 0 for dead
            }
            outFile << endl;
        }

        outFile.close();
        cout << "Game saved to " << file << endl;

    }

    void findLowestERNForBlock() {
        BlockPattern<Cell> blockPattern;
        int minERN = INT_MAX;
        int bestRows = 0, bestColumns = 0, bestAliveCells = 0;
        Grid<Cell>* grid = nullptr; // Declare grid pointer

        // Experiment with different grid sizes
        for (int rows = 2; rows <= 50; rows += 1) { // Start at 2x2 grid, increment by 1
            for (int columns = 2; columns <= 50; columns += 1) {
                for (int aliveCells = 1; aliveCells <= (rows * columns) / 2; aliveCells++) { // Test with different number of alive cells

                    // Clean up the previous grid to avoid memory leaks
                    if (grid) {
                        delete grid;
                        grid = nullptr;
                    }

                    // Initialize grid
                    grid = new Grid<Cell>(rows, columns); // Dynamically allocate a new grid
                    grid->initializeGrid(aliveCells); // Randomly populate grid with alive cells

                    // Debugging output to show the current experiment details
                    std::cout << "Checking Grid Size: " << rows << "x" << columns
                        << ", Alive Cells: " << aliveCells << std::endl;

                    // Check if Block pattern is detected
                    if (blockPattern.detectPattern(*grid)) { // Pass grid by reference
                        int ern = rows + columns + aliveCells;  // Calculate ERN
                        std::cout << "Block pattern detected! ERN: " << ern << std::endl; // Debugging output when pattern is found
                        if (ern < minERN) {
                            minERN = ern;
                            bestRows = rows;
                            bestColumns = columns;
                            bestAliveCells = aliveCells;
                        }
                    }
                    else {
                        std::cout << "No Block pattern detected for this configuration." << std::endl; // Debugging output if no pattern is found
                    }
                }
            }
        }

        // Output the lowest ERN and corresponding grid size
        if (minERN < INT_MAX) {
            std::cout << "Lowest ERN for Block pattern: " << minERN << std::endl;
            std::cout << "Best Grid Size: " << bestRows << "x" << bestColumns << std::endl;
            std::cout << "Number of Alive Cells: " << bestAliveCells << std::endl;
        }
        else {
            std::cout << "Block pattern not found within the tested grid sizes." << std::endl;
        }

        // Clean up at the end of the function to avoid memory leaks
        if (grid) {
            delete grid;
            grid = nullptr;
        }
    }


   


   


    //void displayMenu() {
    //    int mainChoice;
    //    int choice;
    //    string filename;
    //    cout << "===== Main Menu =====" << endl;
    //    cout << "1. Question 1: Conway's Game of Life" << endl;
    //    cout << "2. Question 2: Run experiment to find a Block or Beehive" << endl;
    //    cout << "3. Question 3: Run experiment to find a Blinker or Toad" << endl;
    //    cout << "4. Question 4: Run experiment to find a Glider or LWSS" << endl;
    //    cout << "5. Question 5: Finding ERN" << endl;
    //    cout << "6. Exit" << endl;
    //    cout << "Enter your choice: ";

    //    cin >> mainChoice;

    //    switch (mainChoice) {
    //    case 1:
    //        while (true) {
    //            cout << "===== Conway's Game of Life =====" << endl;
    //            cout << "1. Start a new game" << endl;
    //            cout << "2. Load game" << endl;
    //            cout << "3. Exit" << endl;
    //            cout << "Enter your choice: ";
    //            cin >> choice;

    //            switch (choice) {
    //            case 1:
    //                start();  // Start a new game
    //                simulate();
    //                break;
    //            case 2:
    //                cout << "Enter the filename to load from: ";
    //                cin >> filename;
    //                loadGame(filename);  // Load the game from the given filename
    //                simulate();  // After loading, simulate the game
    //                break;
    //            case 3:
    //                cout << "Exiting game." << endl;
    //                return;
    //            default:
    //                cout << "Invalid choice! Try again." << endl;
    //                break;
    //            }
    //        }
    //        break;
    //    case 2:
    //        runExperiment2();
    //        break;
    //    case 3:
    //        runExperiment3();
    //        break;
    //    case 4:
    //        runExperiment4();
    //        break;
    //    case 5:
    //        while (true) {
    //            cout << "1. Block" << endl;
    //            cout << "2. Beehive" << endl;
    //            cout << "3. Blinker" << endl;
    //            cout << "4. Toad" << endl;
    //            cout << "5. Glider " << endl;
    //            cout << "6. LWSS " << endl;
    //            cin >> choice;

    //            switch (choice) {
    //            case 1:
    //                findLowestERNForBlock();
    //                break;
    //            case 2:
    //              
    //                break;
    //            case 3:
    //               
    //                return;

    //            case 4:

    //                return;

    //            case 5:

    //                return;

    //            case 6:
    //                return;

    //            default:
    //                cout << "Invalid choice! Try again." << endl;
    //                break;
    //            }
    //        }


    //    default:
    //        cout << "Invalid choice! Try again." << endl;
    //        break;

    //    }

    //}

void displayMenu() {
    int mainChoice;
    int choice;
    string filename;
    cout << "===== Main Menu =====" << endl;
    cout << "1. Question 1: Conway's Game of Life" << endl;
    cout << "2. Question 2: Run experiment to find a Block or Beehive" << endl;
    cout << "3. Question 3: Run experiment to find a Blinker or Toad" << endl;
    cout << "4. Question 4: Run experiment to find a Glider or LWSS" << endl;
    cout << "5. Question 5: Finding ERN" << endl;
    cout << "6. Exit" << endl;
    cout << "Enter your choice: ";

    cin >> mainChoice;

    switch (mainChoice) {
    case 1:
        while (true) {
            cout << "===== Conway's Game of Life =====" << endl;
            cout << "1. Start a new game" << endl;
            cout << "2. Load game" << endl;
            cout << "3. Exit" << endl;
            cout << "Enter your choice: ";
            cin >> choice;

            switch (choice) {
            case 1:
                start();  // Start a new game
                simulate();
                break;
            case 2:
                cout << "Enter the filename to load from: ";
                cin >> filename;
                loadGame(filename);  // Load the game from the given filename
                simulate();  // After loading, simulate the game
                break;
            case 3:
                cout << "Exiting game." << endl;
                return;
            default:
                cout << "Invalid choice! Try again." << endl;
                break;
            }
        }
        break;
    case 2:
        runExperiment2();
        break;
    case 3:
        runExperiment3();
        break;
    case 4:
        runExperiment4();
        break;
    case 5:
        while (true) {
            cout << "===== ERN Calculation =====" << endl;
            cout << "1. Block" << endl;
            cout << "2. Beehive" << endl;
            cout << "3. Blinker" << endl;
            cout << "4. Toad" << endl;
            cout << "5. Glider" << endl;
            cout << "6. LWSS" << endl;
            cout << "7. Exit to main menu" << endl;  // Allow user to exit the ERN loop
            cout << "Enter your choice: ";
            cin >> choice;

            switch (choice) {
            case 1:
                cout << "1 is called" << endl;
                findLowestERNForBlock();
                break;
            case 2:
                // Call method to find ERN for Beehive
                break;
            case 3:
                // Call method to find ERN for Blinker
                break;
            case 4:
                // Call method to find ERN for Toad
                break;
            case 5:
                // Call method to find ERN for Glider
                break;
            case 6:
                // Call method to find ERN for LWSS
                break;
            case 7:
                return;  // Exit to the main menu
            default:
                cout << "Invalid choice! Try again." << endl;
                break;
            }
        }
        break;
    case 6:
        cout << "Exiting program." << endl;
        exit(0);
        break;
    default:
        cout << "Invalid choice! Try again." << endl;
        break;
    }
}



    void runExperiment2() {
        BlockPattern<Cell> BlockPattern;
        BeehivePattern<Cell> BeehivePattern;
        int experimentCount = 0;
        bool patternFound = false;

        rows = 30;
        columns = 30;
        numAliveCells = 150;

        const int maxIterationsPerExperiment = 100;  // Safeguard to prevent infinite loops
        const int maxExperiments = 1000;  // Limit the number of total experiments

        std::srand(static_cast<unsigned int>(std::time(0)));  // Seed random generator


        while (!patternFound && experimentCount < maxExperiments) {
            experimentCount++;

            if (grid) {
                delete grid;
            }
            grid = new Grid<Cell>(rows, columns);
            grid->initializeGrid(numAliveCells);
            cout << "Starting experiment " << experimentCount << endl;  // Debugging output

            int iterationCount = 0;
            bool isSame = false;
            Grid<Cell>* previousGrid = new Grid<Cell>(rows, columns); // previous grid

            vector<Grid<Cell>> historyGrids;  // To store grids from each iteration (by value, no pointers)
            historyGrids.push_back(*grid);  // Store the initial grid

            while (!patternFound && iterationCount < maxIterationsPerExperiment && !isSame) {
                iterationCount++;

                *previousGrid = *grid;

                grid->updateGrid();
                historyGrids.push_back(*grid);  // Store the updated grid (by value)


                if (*grid == *previousGrid) {
                    cout << "Grid stabilized after " << iterationCount << " iterations in experiment " << experimentCount << "." << endl;
                    isSame = true;
                }
                cout << "Iteration " << iterationCount << " for experiment " << experimentCount << endl;

                if (BeehivePattern.detectPattern(*grid)) {
                    patternFound = true;
                    cout << "BeeHive pattern found after " << experimentCount << " Experiment(s) and " <<
                        iterationCount << "step(s)!" << endl;
                    break;
                }
                if (BlockPattern.detectPattern(*grid)) {
                    patternFound = true;
                    cout << "Block pattern found after " << experimentCount << " Experiment(s) and " <<
                        iterationCount << "step(s)!" << endl;
                    break;
                }
            }
            if (patternFound) {
                cout << "Pattern found! Displaying the grids from the start to the point where the pattern is detected:" << endl;
                for (size_t idx = 0; idx < historyGrids.size(); ++idx) {
                    cout << "Grid at iteration " << idx << ":" << endl;
                    cout << historyGrids[idx];  // Print each stored grid
                }
            }
            else {
                cout << "No block pattern found in experiment " << experimentCount
                    << " after " << iterationCount << " iterations." << endl;


            }
            if (iterationCount >= maxIterationsPerExperiment) {
                cout << "Maximum iterations reached for experiment " << experimentCount
                    << ". Continuing to the next experiment." << endl;
            }


            historyGrids.clear();  // Clear the history after each experiment
            delete previousGrid;
            delete grid;
            grid = nullptr;
        }
        if (!patternFound) {
            cout << "Maximum number of experiments reached without finding a Block pattern." << endl;
        }

    }


    void runExperiment3() {
        BlinkerPattern<Cell> BlinkerPattern;
        ToadPattern<Cell> ToadPattern;

        int experimentCount = 0;
        bool patternFound = false;

        rows = 30;
        columns = 30;
        numAliveCells = 150;

        const int maxIterationsPerExperiment = 100;
        const int maxExperiments = 100;
        std::srand(static_cast<unsigned int>(std::time(0)));

        while (!patternFound && experimentCount < maxExperiments) {
            experimentCount++;

            if (grid) {
                delete grid;
            }
            grid = new Grid<Cell>(rows, columns);
            grid->initializeGrid(numAliveCells);

            cout << "Starting experiment " << experimentCount << endl;
            int iterationCount = 0;
            bool isSame = false;
            Grid<Cell>* previousGrid = new Grid<Cell>(rows, columns);

            vector<Grid<Cell>> historyGrids;  // To store grids from each iteration (by value, no pointers)
            historyGrids.push_back(*grid);  // Store the initial grid

            while (!patternFound && iterationCount < maxIterationsPerExperiment && !isSame) {
                iterationCount++;
                *previousGrid = *grid;
                grid->updateGrid();
                historyGrids.push_back(*grid);  // Store the updated grid (by value)


                if (*grid == *previousGrid) {
                    cout << "Grid stabilized after " << iterationCount << " iterations in experiment " << experimentCount << "." << endl;
                    isSame = true;
                }
                cout << "Iteration " << iterationCount << " for experiment " << experimentCount << endl;
                if (BlinkerPattern.detectPattern(*grid)) {
                    patternFound = true;
                    cout << "Blinker pattern found after " << experimentCount << " Experiment(s) and " <<
                        iterationCount << "step(s)!" << endl;
                    break;
                }
                if (ToadPattern.detectPattern(*grid)) {
                    patternFound = true;
                    cout << "Toad pattern found after " << experimentCount << " Experiment(s) and " <<
                        iterationCount << "step(s)!" << endl;
                    break;
                }
            }
            if (patternFound) {
                cout << "Pattern found! Displaying the grids from the start to the point where the pattern is detected:" << endl;
                for (size_t idx = 0; idx < historyGrids.size(); ++idx) {
                    cout << "Grid at iteration " << idx << ":" << endl;
                    cout << historyGrids[idx];  // Print each stored grid
                }
            }
            else {
                cout << "No Blinker/Toad pattern found in experiment " << experimentCount
                    << " after " << iterationCount << " iterations." << endl;
            }

            if (iterationCount >= maxIterationsPerExperiment) {
                cout << "Maximum iterations reached for experiment " << experimentCount
                    << ". Continuing to the next experiment." << endl;
            }
            delete previousGrid;
            delete grid;
            grid = nullptr;
        }
        if (!patternFound) {
            cout << "Maximum number of experiments reached without finding a Blinker pattern." << endl;
        }

    }



    void runExperiment4() {
        GliderPattern<Cell> GliderPattern;
        LWSSPattern<Cell> LWSSPattern;


        int experimentCount = 0;
        bool patternFound = false;

        rows = 30;
        columns = 30;
        numAliveCells = 150;

        const int maxIterationsPerExperiment = 100;
        const int maxExperiments = 1000;
        std::srand(static_cast<unsigned int>(std::time(0)));



        while (!patternFound && experimentCount < maxExperiments) {
            experimentCount++;

            if (grid) {
                delete grid;
            }


            grid = new Grid<Cell>(rows, columns);
            grid->initializeGrid(numAliveCells);
            //initializeLWSS1Pattern();
            //cout << *grid;

            cout << "Starting experiment " << experimentCount << endl;
            int iterationCount = 0;
            bool isSame = false;
            Grid<Cell>* previousGrid = new Grid<Cell>(rows, columns);

            vector<Grid<Cell>> historyGrids;  // To store grids from each iteration (by value, no pointers)
            historyGrids.push_back(*grid);  // Store the initial grid

            while (!patternFound && iterationCount < maxIterationsPerExperiment && !isSame) {
                iterationCount++;
                *previousGrid = *grid;
                grid->updateGrid();
                historyGrids.push_back(*grid);  // Store the updated grid (by value)
                // comparing current grid with previous grid
                if (*grid == *previousGrid) {
                    cout << "Grid stabilized after " << iterationCount << " iterations in experiment " << experimentCount << "." << endl;
                    isSame = true;
                }
                cout << "Iteration " << iterationCount << " for experiment " << experimentCount << endl;

                if (GliderPattern.detectPattern(*grid)) {
                    patternFound = true;
                    cout << "Glider pattern found after " << experimentCount << " Experiment(s) and " <<
                        iterationCount << "step(s)!" << endl;
                    break;
                }
                if (LWSSPattern.detectPattern(*grid)) {
                    patternFound = true;
                    cout << "LWSS pattern found after " << experimentCount << " Experiment(s) and " <<
                        iterationCount << "step(s)!" << endl;
                    break;
                }
            }
            if (patternFound) {
                cout << "Pattern found! Displaying the grids from the start to the point where the pattern is detected:" << endl;
                for (size_t idx = 0; idx < historyGrids.size(); ++idx) {
                    cout << "Grid at iteration " << idx << ":" << endl;
                    cout << historyGrids[idx];  // Print each stored grid
                }
            }
            else {
                cout << "No Glider/LWSS pattern found in experiment " << experimentCount
                    << " after " << iterationCount << " iterations." << endl;
            }
            if (iterationCount >= maxIterationsPerExperiment) {
                cout << "Maximum iterations reached for experiment " << experimentCount
                    << ". Continuing to the next experiment." << endl;
            }
            delete previousGrid;
            delete grid;
            grid = nullptr;
        }
        if (!patternFound) {
            cout << "Maximum number of experiments reached without finding a Blinker pattern." << endl;
        }
    }
};

int main() {
    Game game;
    game.displayMenu();
    return 0;
}




