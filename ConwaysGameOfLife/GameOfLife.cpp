using namespace std;

#include <iostream>
#include <vector>
#include <unordered_map>
#include <cstdlib>  
#include <ctime>  
#include <fstream>  
#include <thread>
#include <mutex>

// Declare a mutex to protect shared resources
std::mutex gridMutex;
std::mutex ernMutex;


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

    std::srand(static_cast<unsigned int>(std::time(0)));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            grid[i][j].setDead();

        }
    }

    int aliveCount = 0;
    while (aliveCount < numAliveCells) {
        int randomRow = std::rand() % rows;
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
        return false;
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            if (grid[i][j].isAlive() != other.grid[i][j].isAlive()) {
                return false;
            }
        }
    }
    return true;
}

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
                    os << "O";
                }
                else {
                    os << " ";
                }
            }
            else {
                os << " ";
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
                if (aliveNeighbors < 2 || aliveNeighbors > 3) {
                    newGrid[i][j].setDead();
                }
            }
            else {
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
                        return true;
                    }
                }
            }
        }
        return false;
    }


};
template<typename T>
class BeehivePattern : public Pattern<T> {
public:
    BeehivePattern() : Pattern<T>("Beehive") {}

    bool detectPattern(const Grid<T>& grid) const override {
        for (int i = 0; i < grid.getRows() - 4; i++) {
            for (int j = 0; j < grid.getColumns() - 4; j++) {
                if (doesHorizontalBeehiveExist(grid, i, j) || doesVerticalBeehiveExist(grid, i, j)) {
                    return true;
                }
            }
        }
        return false;
    }

private:
    bool doesHorizontalBeehiveExist(const Grid<T>& grid, int row, int col) const {
        if (row + 2 >= grid.getRows() || col + 3 >= grid.getColumns()) return false;
        return
            grid.getCellState(row, col + 1)
            && grid.getCellState(row, col + 2)
            && grid.getCellState(row + 1, col)
            && grid.getCellState(row + 1, col + 3)
            && grid.getCellState(row + 2, col + 1)
            && grid.getCellState(row + 2, col + 2)
            && grid.countAliveNeighbors(row, col + 1) == 2
            && grid.countAliveNeighbors(row, col + 2) == 2
            && grid.countAliveNeighbors(row + 1, col) == 2
            && grid.countAliveNeighbors(row + 1, col + 3) == 2
            && grid.countAliveNeighbors(row + 2, col + 1) == 2
            && grid.countAliveNeighbors(row + 2, col + 2) == 2;
    }
    bool doesVerticalBeehiveExist(const Grid<T>& grid, int row, int col) const {
        if (row + 2 >= grid.getRows() || col + 3 >= grid.getColumns()) return false;
        return
            grid.getCellState(row, col + 1)
            && grid.getCellState(row + 1, col)
            && grid.getCellState(row + 1, col + 2)
            && grid.getCellState(row + 2, col)
            && grid.getCellState(row + 2, col + 2)
            && grid.getCellState(row + 3, col + 1)
            && grid.countAliveNeighbors(row, col + 1) == 2
            && grid.countAliveNeighbors(row + 1, col) == 2
            && grid.countAliveNeighbors(row + 1, col + 2) == 2
            && grid.countAliveNeighbors(row + 2, col) == 2
            && grid.countAliveNeighbors(row + 2, col + 2) == 2
            && grid.countAliveNeighbors(row + 3, col + 1) == 2;
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
                    Grid<T> nextGrid = grid;
                    nextGrid.updateGrid();
                    nextGrid.updateGrid();

                    if (doesVerticalBlinkerExist(nextGrid, i, j)) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

private:
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
        for (int i = 0; i < grid.getRows() - 4; i++) {
            for (int j = 0; j < grid.getColumns() - 4; j++) {
                if (doesToadExist(grid, i, j)) {
                    Grid<T> nextGrid = grid;
                    nextGrid.updateGrid();
                    nextGrid.updateGrid();
                    if (doesToadExist(nextGrid, i, j)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

private:
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
                if (doesGliderExist(grid, i, j)) {
                    Grid<T> nextGrid = grid;
                    for (int step = 0; step < 4; step++) {
                        nextGrid.updateGrid();
                    }

                    if ((i + 1 < nextGrid.getRows()) && (j + 1 < nextGrid.getColumns()) &&
                        (i - 1 >= 0) && (j - 1 >= 0)) {
                        if (doesGlider1Exist(nextGrid, i + 1, j + 1)
                            || doesGlider2Exist(nextGrid, i - 1, j + 1)
                            || doesGlider3Exist(nextGrid, i + 1, j - 1)
                            || doesGlider4Exist(nextGrid, i - 1, j - 1)) {
                            return true;
                        }
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

            grid.countAliveNeighbors(row + 1, col) == 1 &&
            grid.countAliveNeighbors(row, col + 1) == 3 &&
            grid.countAliveNeighbors(row, col + 2) == 2 &&
            grid.countAliveNeighbors(row + 1, col + 2) == 3 &&
            grid.countAliveNeighbors(row + 2, col + 2) == 1;
    }

    bool doesGlider3Exist(const Grid<T>& grid, int row, int col) const {

        if (row + 2 >= grid.getRows() || col + 2 >= grid.getColumns()) {
            return false;
        }

        return
            grid.getCellState(row, col) &&
            grid.getCellState(row + 1, col) &&
            grid.getCellState(row + 2, col) &&
            grid.getCellState(row + 2, col + 1) &&
            grid.getCellState(row + 1, col + 2) &&
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
            for (int j = 0; j < grid.getColumns() - 4; j++) {
                if (doesLWSS1Exist(grid, i, j) || doesLWSS2Exist(grid, i, j) || doesLWSS3Exist(grid, i, j)
                    || doesLWSS4Exist(grid, i, j) || doesLWSS5Exist(grid, i, j)
                    || doesLWSS6Exist(grid, i, j) || doesLWSS7Exist(grid, i, j) || doesLWSS8Exist(grid, i, j)) {
                    return true;
                    /*Grid<T> nextGrid = grid;
                    for (int step = 0; step < 4; ++step) {
                        nextGrid.updateGrid();
                    }
                    if ((i + 2 < nextGrid.getRows()) && (j + 2 < nextGrid.getColumns()) &&
                        (i - 2 >= 0) && (j - 2 >= 0)) {

                        if (doesLWSS1Exist(nextGrid, i, j - 2)
                            || doesLWSS2Exist(nextGrid, i, j + 2)
                            || doesLWSS3Exist(nextGrid, i, j + 2)
                            || doesLWSS4Exist(nextGrid, i, j - 2)
                            || doesLWSS5Exist(nextGrid, i - 2, j)
                            || doesLWSS6Exist(nextGrid, i - 2, j)
                            || doesLWSS7Exist(nextGrid, i - 2, j)
                            || doesLWSS8Exist(nextGrid, i + 2, j)) {
                            return true;
                        }
                    }*/
                }
            }
        }
        return false;
    }


private:
    bool doesLWSS1Exist(const Grid<T>& grid, int row, int col) const {
        if (row + 3 >= grid.getRows() || col + 4 >= grid.getColumns()) {
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
        if (row + 4 >= grid.getRows() || col + 3 >= grid.getColumns()) {
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
        if (row + 4 >= grid.getRows() || col + 3 >= grid.getColumns()) {
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
        if (row + 4 >= grid.getRows() || col + 3 >= grid.getColumns()) {
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
        if (row + 4 >= grid.getRows() || col + 3 >= grid.getColumns()) {
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
        delete grid;
        delete initialGrid;
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

        stepsCount = 0;
    }

    void restartGame() {
        if (initialGrid != nullptr) {
            *grid = *initialGrid;
            stepsCount = 0;
            cout << "Game has been restarted with the initial configuration." << endl;
        }
        else {
            cout << "No game to restart." << endl;
        }
    }


    void simulate() {
        string continueOption = "y";

        cout << "\033[H\033[J";
        cout << *grid;

        while (continueOption == "y" && stepsCount < steps) {

            cout << "Would you like to continue next step (y/n) or save the game (s) ? . To reset press (r): ";
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

                cout << "\033[H";
                cout << *grid;

                if (stepsCount >= steps) {
                    cout << "Maximum steps have been reached. Simulation ended." << endl;
                    break;
                }
            }
            if (continueOption == "r") {
                restartGame();
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
        inFile >> rows >> columns >> stepsCount >> steps;

        delete grid;
        grid = new Grid<Cell>(rows, columns);
        int cellStateInt;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                inFile >> cellStateInt;
                if (cellStateInt == 1) {
                    grid->getCell(i, j).setAlive();
                }
                else {
                    grid->getCell(i, j).setDead();
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
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < columns; ++j) {
                outFile << (grid->getCellState(i, j) ? 1 : 0) << " ";
            }
            outFile << endl;
        }
        outFile.close();
        cout << "Game saved to " << file << endl;

    }

    void runExperiment2() {
        BlockPattern<Cell> BlockPattern;
        BeehivePattern<Cell> BeehivePattern;
        int experimentCount = 0;
        bool patternFound = false;

        rows = 40;
        columns = 40;
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
            cout << "Starting experiment " << experimentCount << endl;

            int iterationCount = 0;
            bool isSame = false;
            Grid<Cell>* previousGrid = new Grid<Cell>(rows, columns);

            vector<Grid<Cell>> historyGrids;
            historyGrids.push_back(*grid);

            while (!patternFound && iterationCount < maxIterationsPerExperiment && !isSame) {
                iterationCount++;

                *previousGrid = *grid;

                grid->updateGrid();
                historyGrids.push_back(*grid);

                if (*grid == *previousGrid) {
                    isSame = true;
                }

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
                    cout << historyGrids[idx];
                }
            }

            historyGrids.clear();
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

        rows = 40;
        columns = 40;
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

            cout << "Starting experiment " << experimentCount << endl;
            int iterationCount = 0;
            bool isSame = false;
            Grid<Cell>* previousGrid = new Grid<Cell>(rows, columns);

            vector<Grid<Cell>> historyGrids;
            historyGrids.push_back(*grid);

            while (!patternFound && iterationCount < maxIterationsPerExperiment && !isSame) {
                iterationCount++;
                *previousGrid = *grid;
                grid->updateGrid();
                historyGrids.push_back(*grid);


                if (*grid == *previousGrid) {
                    isSame = true;
                }
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
                    cout << historyGrids[idx];
                }
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

        rows = 40;
        columns = 40;
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

            cout << "Starting experiment " << experimentCount << endl;
            int iterationCount = 0;
            bool isSame = false;
            Grid<Cell>* previousGrid = new Grid<Cell>(rows, columns);

            vector<Grid<Cell>> historyGrids;
            historyGrids.push_back(*grid);

            while (!patternFound && iterationCount < maxIterationsPerExperiment && !isSame) {
                iterationCount++;
                *previousGrid = *grid;
                grid->updateGrid();
                historyGrids.push_back(*grid);

                if (*grid == *previousGrid) {
                    isSame = true;
                }
                /*if (GliderPattern.detectPattern(*grid)) {
                    patternFound = true;
                    cout << "Glider pattern found after " << experimentCount << " Experiment(s) and " <<
                        iterationCount << "step(s)!" << endl;
                    break;
                }*/
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
                    cout << historyGrids[idx];
                }
            }
            delete previousGrid;
            delete grid;
            grid = nullptr;
        }
        if (!patternFound) {
            cout << "Maximum number of experiments reached without finding a pattern." << endl;
        }
    }


    void findLowestERNForBlock() {
        BlockPattern<Cell> blockPattern;
        int minERN = INT_MAX;
        int bestRows = 0, bestColumns = 0, bestAliveCells = 0;
        Grid<Cell>* grid = nullptr;

        // Launch multiple threads to experiment with different configurations
        auto experimentTask = [&](int startRow, int endRow) {
            for (int rows = startRow; rows <= endRow; rows++) {
                for (int columns = 2; columns <= 10; columns++) {
                    for (int aliveCells = 1; aliveCells <= (rows * columns) / 2; aliveCells++) {
                        for (int trial = 0; trial < 10; trial++) {

                            // Mutex lock for grid manipulation
                            std::lock_guard<std::mutex> lock(gridMutex);

                            if (grid) {
                                delete grid;
                                grid = nullptr;
                            }
                            grid = new Grid<Cell>(rows, columns);
                            grid->initializeGrid(aliveCells);

                            if (blockPattern.detectPattern(*grid)) {
                                int ern = rows + columns + aliveCells;
                                std::cout << "Block pattern detected! ERN: " << ern << std::endl;
                                if (ern < minERN) {
                                    minERN = ern;
                                    bestRows = rows;
                                    bestColumns = columns;
                                    bestAliveCells = aliveCells;
                                }
                                break;
                            }
                        }
                    }
                }
            }
            };

        std::thread t1(experimentTask, 2, 5);
        std::thread t2(experimentTask, 6, 10);

        t1.join();
        t2.join();

        if (minERN < INT_MAX) {
            std::cout << "Lowest ERN for Block pattern: " << minERN << std::endl;
            std::cout << "Best Grid Size: " << bestRows << "x" << bestColumns << std::endl;
            std::cout << "Number of Alive Cells: " << bestAliveCells << std::endl;
        }
        else {
            std::cout << "Block pattern not found within the tested grid sizes." << std::endl;
        }

        if (grid) {
            delete grid;
        }
    }

    void findLowestERNForBeehive() {
        BeehivePattern<Cell> beehivePattern;
        int minERN = INT_MAX;
        int bestRows = 0, bestColumns = 0, bestAliveCells = 0;
        Grid<Cell>* grid = nullptr;

        auto experimentTask = [&](int startRow, int endRow) {
            for (int rows = startRow; rows <= endRow; rows++) {
                for (int columns = 6; columns <= 20; columns++) {
                    for (int aliveCells = 1; aliveCells <= (rows * columns) / 2; aliveCells++) {
                        for (int trial = 0; trial < 10; trial++) {

                            // Mutex lock for grid manipulation
                            std::lock_guard<std::mutex> lock(gridMutex);

                            if (grid) {
                                delete grid;
                                grid = nullptr;
                            }
                            grid = new Grid<Cell>(rows, columns);
                            grid->initializeGrid(aliveCells);

                            if (beehivePattern.detectPattern(*grid)) {
                                int ern = rows + columns + aliveCells;
                                std::cout << "Beehive pattern detected! ERN: " << ern << std::endl;
                                if (ern < minERN) {
                                    minERN = ern;
                                    bestRows = rows;
                                    bestColumns = columns;
                                    bestAliveCells = aliveCells;
                                }
                                break;
                            }
                        }
                    }
                }
            }
            };

        std::thread t1(experimentTask, 6, 13);
        std::thread t2(experimentTask, 14, 20);

        t1.join();
        t2.join();

        if (minERN < INT_MAX) {
            std::cout << "Lowest ERN for Beehive pattern: " << minERN << std::endl;
            std::cout << "Best Grid Size: " << bestRows << "x" << bestColumns << std::endl;
            std::cout << "Number of Alive Cells: " << bestAliveCells << std::endl;
        }
        else {
            std::cout << "Beehive pattern not found within the tested grid sizes." << std::endl;
        }

        if (grid) {
            delete grid;
        }
    }



    void findLowestERNForBlinker() {
        BlinkerPattern<Cell> blinkerPattern;
        int minERN = INT_MAX;
        int bestRows = 0, bestColumns = 0, bestAliveCells = 0;
        Grid<Cell>* grid = nullptr;

        for (int rows = 3; rows <= 10; rows += 1) {
            for (int columns = 3; columns <= 10; columns += 1) {
                for (int aliveCells = 3; aliveCells <= (rows * columns) / 2; aliveCells++) {
                    for (int trial = 0; trial < 10; trial++) {

                        if (grid) {
                            delete grid;
                            grid = nullptr;
                        }

                        grid = new Grid<Cell>(rows, columns);
                        grid->initializeGrid(aliveCells);

                        if (blinkerPattern.detectPattern(*grid)) {
                            int ern = rows + columns + aliveCells;
                            cout << "Blinker pattern detected! ERN: " << ern << std::endl;
                            if (ern < minERN) {
                                minERN = ern;
                                bestRows = rows;
                                bestColumns = columns;
                                bestAliveCells = aliveCells;
                            }
                        }
                    }
                }
            }
        }

        if (minERN < INT_MAX) {
            std::cout << "Lowest ERN for Blinker pattern: " << minERN << std::endl;
            std::cout << "Best Grid Size: " << bestRows << "x" << bestColumns << std::endl;
            std::cout << "Number of Alive Cells: " << bestAliveCells << std::endl;
        }
        else {
            std::cout << "Blinker pattern not found within the tested grid sizes." << std::endl;
        }

        if (grid) {
            delete grid;
            grid = nullptr;
        }
    }

    void findLowestERNForToad() {
        ToadPattern<Cell> toadPattern;
        int minERN = INT_MAX;
        int bestRows = 0, bestColumns = 0, bestAliveCells = 0;
        Grid<Cell>* grid = nullptr;

        for (int rows = 6; rows <= 15; rows += 1) {
            for (int columns = 6; columns <= 15; columns += 1) {
                for (int aliveCells = 6; aliveCells <= (rows * columns) / 2; aliveCells++) {

                    for (int trial = 0; trial < 10; trial++) {

                        if (grid) {
                            delete grid;
                            grid = nullptr;
                        }

                        grid = new Grid<Cell>(rows, columns);
                        grid->initializeGrid(aliveCells);

                        if (toadPattern.detectPattern(*grid)) {
                            int ern = rows + columns + aliveCells;
                            std::cout << "Toad pattern detected! ERN: " << ern << std::endl;
                            if (ern < minERN) {
                                minERN = ern;
                                bestRows = rows;
                                bestColumns = columns;
                                bestAliveCells = aliveCells;
                            }
                        }
                    }
                }
            }
        }

        if (minERN < INT_MAX) {
            std::cout << "Lowest ERN for Toad pattern: " << minERN << std::endl;
            std::cout << "Best Grid Size: " << bestRows << "x" << bestColumns << std::endl;
            std::cout << "Number of Alive Cells: " << bestAliveCells << std::endl;
        }
        else {
            std::cout << "Toad pattern not found within the tested grid sizes." << std::endl;
        }

        if (grid) {
            delete grid;
            grid = nullptr;
        }
    }

    void findLowestERNForGlider() {
        GliderPattern<Cell> gliderPattern;
        int minERN = INT_MAX;
        int bestRows = 0, bestColumns = 0, bestAliveCells = 0;
        Grid<Cell>* grid = nullptr;

        for (int rows = 5; rows <= 15; rows += 1) {
            for (int columns = 5; columns <= 15; columns += 1) {
                for (int aliveCells = 5; aliveCells <= (rows * columns) / 2; aliveCells++) {

                    for (int trial = 0; trial < 10; trial++) {

                        if (grid) {
                            delete grid;
                            grid = nullptr;
                        }

                        grid = new Grid<Cell>(rows, columns);
                        grid->initializeGrid(aliveCells);

                        if (gliderPattern.detectPattern(*grid)) {
                            int ern = rows + columns + aliveCells;
                            std::cout << "Glider pattern detected! ERN: " << ern << std::endl;
                            if (ern < minERN) {
                                minERN = ern;
                                bestRows = rows;
                                bestColumns = columns;
                                bestAliveCells = aliveCells;
                            }
                        }
                    }
                }
            }
        }

        if (minERN < INT_MAX) {
            std::cout << "Lowest ERN for Glider pattern: " << minERN << std::endl;
            std::cout << "Best Grid Size: " << bestRows << "x" << bestColumns << std::endl;
            std::cout << "Number of Alive Cells: " << bestAliveCells << std::endl;
        }
        else {
            std::cout << "Glider pattern not found within the tested grid sizes." << std::endl;
        }

        if (grid) {
            delete grid;
            grid = nullptr;
        }
    }

    void findLowestERNForLWSS() {
        LWSSPattern<Cell> lwssPattern;
        int minERN = INT_MAX;
        int bestRows = 0, bestColumns = 0, bestAliveCells = 0;
        Grid<Cell>* grid = nullptr;

        for (int rows = 11; rows <= 25; rows += 1) {
            for (int columns = 11; columns <= 25; columns += 1) {
                for (int aliveCells = 5; aliveCells <= (rows * columns) / 2; aliveCells++) {


                    for (int trial = 0; trial < 10; trial++) {

                        if (grid) {
                            delete grid;
                            grid = nullptr;
                        }

                        grid = new Grid<Cell>(rows, columns);
                        grid->initializeGrid(aliveCells);

                        if (lwssPattern.detectPattern(*grid)) {
                            int ern = rows + columns + aliveCells;
                            std::cout << "LWSS pattern detected! ERN: " << ern << std::endl;
                            if (ern < minERN) {
                                minERN = ern;
                                bestRows = rows;
                                bestColumns = columns;
                                bestAliveCells = aliveCells;
                            }
                        }
                    }
                }
            }
        }

        if (minERN < INT_MAX) {
            std::cout << "Lowest ERN for LWSS pattern: " << minERN << std::endl;
            std::cout << "Best Grid Size: " << bestRows << "x" << bestColumns << std::endl;
            std::cout << "Number of Alive Cells: " << bestAliveCells << std::endl;
        }
        else {
            std::cout << "LWSS pattern not found within the tested grid sizes." << std::endl;
        }

        if (grid) {
            delete grid;
        }

    }


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
                    start();
                    simulate();
                    break;
                case 2:
                    cout << "Enter the filename to load from: ";
                    cin >> filename;
                    loadGame(filename);
                    simulate();
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
                cout << "7. Exit" << endl;
                cout << "Enter your choice: ";
                cin >> choice;

                switch (choice) {
                case 1:
                    cout << "1 is called" << endl;
                    findLowestERNForBlock();
                    break;
                case 2:
                    findLowestERNForBeehive();
                    break;
                case 3:
                    findLowestERNForBlinker();
                    break;
                case 4:
                    findLowestERNForToad();
                    break;
                case 5:
                    findLowestERNForGlider();
                    break;
                case 6:
                    findLowestERNForLWSS();
                    break;
                case 7:
                    return;
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

};

int main() {
    Game game;
    game.displayMenu();
    return 0;
}




