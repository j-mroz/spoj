#include <cstdio>
#include <utility>
#include <queue>

using std::queue;
using std::pair;
using std::make_pair;
typedef pair<unsigned int, unsigned int> position_t;

// See http://www.spoj.com/problems/BITMAP/
class Algorithm {
    // Node bitmap
    char bitmap_[182][182];

    // Visited nodes
    // bool visited_[182][182];

    // Current known distance to white node
    int distance_[182][182];

    // Number of lines
    int lines_count_;
    // Number of columns
    int columns_count_;

    // FIFO queue of nodes for BFS
    queue<position_t> node_queue_;

    const static char WHITE = 1;

public:
    void readInput();
    void run();
    void printOutput();
} algo;

/**
 * Runs the algorithm - based on BFS
 */
void Algorithm::run() {

    // Find all white nodes and add them to priority queue as starting points
    // Also resets visited and distance arrays
    for (int line = 0; line < lines_count_; ++line) {
        for (int column = 0; column < columns_count_; ++column) {
            //visited_[line][column] = false;
            distance_[line][column] = 5000;
            if (bitmap_[line][column] == WHITE) {
                node_queue_.push( position_t(line, column) );
                distance_[line][column] = 0;
            }
        }
    }

    // Iterate as long as we have nodes in queue
    while (!node_queue_.empty()) {

        // Pop the first node
        position_t node  = node_queue_.front();
        node_queue_.pop();

        // And try to move in every possible direction from this node
        int directions[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
        for (int d = 0; d < 4; ++d) {
            int line = node.first + directions[d][0];
            int column = node.second + directions[d][1];

            // Validate direction
            if (line < 0 || line >= lines_count_ || column < 0 || column >= columns_count_)
                continue;

            // Check if that direction is better that currently known
            if (distance_[line][column] <= distance_[node.first][node.second]+1)
                continue;

            // Set new distance and add to queue
            distance_[line][column] = distance_[node.first][node.second]+1;            
            node_queue_.push(position_t(line, column));
        }
    }
}

/**
 * Parse input into Algorithm
 */
void Algorithm::readInput() {
    scanf("%u %u", &lines_count_, &columns_count_);
    for (int line = 0; line < lines_count_; ++line) {
        scanf("%s", bitmap_[line]);
        for (int column = 0; column < columns_count_; ++column)
            bitmap_[line][column] -= '0';
    }
}

/**
 * Prints the output
 */
void Algorithm::printOutput() {
    for (int line = 0; line < lines_count_; ++line) {
        for (int column = 0; column < columns_count_; ++column)
            printf("%d ", distance_[line][column]);
        printf("\n");
    }
}


/**
 * Main loop
 */
void run_tests_loop() {
    int test_number = 0;
    scanf("%d", &test_number);
    while (test_number--) {
		algo.readInput();
		algo.run();
		algo.printOutput();
    }
}

/**
 * Main entry point
 */
int main() {
    run_tests_loop();
    return 0;
}
