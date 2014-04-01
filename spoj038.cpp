#include <cstdio>
#include <iostream>
#include <utility>
#include <queue>
#include <vector>
#include <set>
#include <list>

using std::pair;
using std::max;
using std::min;
using std::vector;

// (row, column)
typedef pair<short, short> node_t;

// (depth, span)
typedef pair<short, short> tree_data_t;

class Algorithm {
    const static int GRID_SIZE = 1000;

    char grid_[GRID_SIZE][GRID_SIZE];
    bool visited_[GRID_SIZE][GRID_SIZE];
    //node_t parent_[GRID_SIZE][GRID_SIZE];

    int rows_count_;
    int columns_count_;
    int rope;

    const static char BLOCKED = '#';
    const static char FREE = '.';

    int dfsVisit(short node_row, short node_column, short x);
    inline bool isBlocked(int row, int column);

public:
    void readInput();
    void run();
    void printOutput();
} algo;


int Algorithm::dfsVisit(short node_row, short node_column, short x) {

    visited_[node_row][node_column] = true;
    int max_depth1 = 0;
    int max_depth2 = 0;

    // For each children
    int delta[][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
    for (int move = 0; move < 4; ++move) {

        // Check boundaries of grid
        int row = node_row + delta[move][0];
        int column = node_column + delta[move][1];
        if (column < 0 || column >= columns_count_ || row < 0 || row >= rows_count_)
            continue;

        // Check if it's not blocked way
        // and if node already have a parrent
        if (isBlocked(row, column) || visited_[row][column])
            continue;

        int depth = dfsVisit(row, column, 1);

        if (depth > max_depth1) {
            max_depth2 = max_depth1;
            max_depth1 = depth;
        } else if (depth > max_depth2) {
            max_depth2 = depth;
        }
        rope = max(rope, max_depth1 + max_depth2);
    }

    return max_depth1 + x;
}


bool Algorithm::isBlocked(int row, int column) {
    return grid_[row][column] == BLOCKED;
}

/**
 * Runs the algorithm - based on BFS
 */
void Algorithm::run() {

    for (int row = 0; row < rows_count_; ++row)
        for (int column = 0; column < columns_count_; ++column)
            if (grid_[row][column] == BLOCKED)
                visited_[row][column] = true;
            else
                visited_[row][column] = false;

    // Find not blocked node
    short start_row;
    short start_column;
    for (int row = 0; row < rows_count_; ++row) {
        for (int column = 0; column < columns_count_; ++column) {
            if (grid_[row][column] == FREE) {
                start_row = row;
                start_column = column;
                goto start_node_found;
            }
        }
    }

start_node_found:
    rope = 0;
    tree_data_t data;
    dfsVisit(start_row, start_column, 0);
}


/**
 * Parse input into Algorithm
 */
void Algorithm::readInput() {
    scanf("%d %d", &columns_count_, &rows_count_);
    if (columns_count_ > GRID_SIZE || rows_count_ > GRID_SIZE)
        return;
    for (int line = 0; line < rows_count_; ++line) {
        scanf("%s", grid_[line]);
        // for (int column = 0; column < columns_count_; ++column)
            // parent_[line][column] = node_t(-1, -1);
    }
}

/**
 * Prints the output
 */
void Algorithm::printOutput() {
    // for (int line = 0; line < rows_count_; ++line) {
    //     printf("%s\n", grid_[line]);
    // }
    // Print tree span
    //printf("Maximum rope length is %d.\n", tree_span(tree_info_));
    std::cout << "Maximum rope length is " << rope << "." << std::endl;
}

/**
 * Run single test case
 */
void run_testcase() {
    algo.readInput();
    algo.run();
    algo.printOutput();
}

/**
 * Main loop
 */
void run_tests_loop() {
    int test_number = 0;
    scanf("%d", &test_number);
    while (test_number--) {
        run_testcase();
    }
}

/**
 * Main entry point
 */
int main() {
    run_tests_loop();
    return 0;
}