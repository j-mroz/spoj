#include <cstdio>
#include <queue>
#include <set>
#include <bitset>
#include <numeric>
#include <iostream>
#include <vector>

// Use only what is neded
using std::queue;
using std::set;
using std::accumulate;
//using std::bitset;
using std::vector;

int gcd(int a, int b){
    for (;;) {
        if (a == 0) return b;
        b %= a;
        if (b == 0) return a;
        a %= b;
    }
}

int lcm(int a, int b) {
    int temp = gcd(a, b);
    return temp ? (a / temp * b) : 0;
}


struct Node {
	int time;
	int line;
	int column;
};

struct Algorithm {

	// Initialize
	void init();

	// Run algorithm
	void run();

	// Check if move is valid
	inline bool moveValid(Node& current, int direction[2]);

	// Reads inut from stdin
	void readInput();

	// Print output to stdout
	void printOutput();

	const static char BLOCKED = '*';
	const static char FREE = '.';
	const static int GRID_SIZE = 25;
	const static int TIME_SIZE = 9;

	// Grid of graph nodes
	char grid_[GRID_SIZE][GRID_SIZE];

	// Schedule of work change
	char schedule_[GRID_SIZE][GRID_SIZE];

	// Grid point at given time
	//std::vector<bitset<25*25> > visited_;
	vector<vector<char> > visited_;
	vector<vector<char> > visit_count_;

	// Size of grid and schedule
	int grid_size_;

	// Lowest commonon multiple
	int lcm_;

	// BFS queue
	queue<Node> visit_queue_;

	// set of unique periods
	set<int> periods_;

	bool end_reached_;
	int time_spent_;

} algo;

void Algorithm::init() {
	periods_.clear();
	std::queue<Node> empty;
	std::swap(visit_queue_, empty);
	visited_.clear();
	//visited_.resize(5040, bitset<25*25>(0) );
	//visited_.resize(841, vector<char>(25*25, 0) );
	visited_.resize(5040, vector<char>(25*25, 0) );

	visit_count_.clear();
	visit_count_.resize(26, vector<char>(26, 0) );

	end_reached_ = false;
	time_spent_ = 0;
	for (int line = 0; line < grid_size_; ++line) {
		for (int column = 0; column < grid_size_; ++column) {
			if (schedule_[line][column] > 0)
				periods_.insert(schedule_[line][column]);
		}
	}
	lcm_ = accumulate(periods_.begin(), periods_.end(), 1, lcm);
}

/**
 * Runs the algoritm
 */
void Algorithm::run() {

	if (grid_[grid_size_-1][grid_size_-1] == BLOCKED && schedule_[grid_size_-1][grid_size_-1] == 0)
		return;

	// Enqueue start node into queue
	Node node = { 0, 0, 0 };
	visit_queue_.push(node);

	
	// Carry on as long as there are nodes to process
	while (!visit_queue_.empty()) {

		// Get the first element from queue
		node = visit_queue_.front();
		visit_queue_.pop();
		
		// End of algorithm
		if (node.line == grid_size_-1 && node.column == grid_size_-1) {
			end_reached_ = true;
			time_spent_ = node.time;
			break;
		}
		//std::cout << node.line << ", " << node.column << std::endl;
		//if (node.line == 4 && node.column == 4)
		//	std::cout << "Debug on\n";

		// Enqueue each possible move
		int directions[5][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {0, 0} };
		const int wait_in_place = 4;
		for (int move = 0; move < 5; ++move) {

			// Check if move is valid
			if (!moveValid(node, directions[move]))
				continue;
			

			// Prpapre Node
			Node next = node;
			next.line += directions[move][0];
			next.column += directions[move][1];
			next.time += 1;
			if (visit_count_[next.line][next.column] > 10) {
				continue;
			}
			visit_count_[next.line][next.column]++;

			// Enqueue and mark already as visited
			visit_queue_.push(next);
			
			visited_[next.time][next.line*GRID_SIZE + next.column] = 1;
		}
	}
}

bool Algorithm::moveValid(Node& current, int direction[2]) {
	int line = current.line + direction[0];
	int column = current.column + direction[1];
	int time = current.time + 1;

	// Check if planed move is in bonds of grid
	if (line < 0 || line >= grid_size_ || column < 0 || column >= grid_size_)
		return false;
	
	// Check if it was already visited
	if (visited_[time%lcm_][line*GRID_SIZE + column] ) {
		return false;
	}

	// Check if planed node is free in time of this move
	// time=<0, period> -> oryginal_state
	// time=<period+1, period-1> ->  !oryginal_state

	int period = schedule_[line][column];
	bool state_switch = (period > 0) && ((time/period)%2);
	
	if (state_switch && grid_[line][column] == FREE)
		return false;
	if (!state_switch && grid_[line][column] == BLOCKED)
		return false;
	if (grid_[line][column] == BLOCKED && schedule_[line][column] == 0)
		return false;


	return true;
}

/**
 * Read input
 */
void Algorithm::readInput() {
	scanf("%d\n", &grid_size_);
	for (int i = 0; i < grid_size_; ++i) {
		scanf("%s\n", grid_[i]);
	}
	for (int i = 0; i < grid_size_; ++i) {
		scanf("%s\n", schedule_[i]);
		for (int j = 0; j < grid_size_; ++j) {
			schedule_[i][j] -= '0';
		}
	}
}

void Algorithm::printOutput() {
	if (end_reached_) 
		printf("%d\n", time_spent_);
	else
		printf("NO\n");
}

/**
 * Runs particular testcase
 */
void run_testcase() {
	algo.readInput();
	algo.init();
	algo.run();
	algo.printOutput();
}

/**
 * Main test loop
 */
void run_test_loop() {
	int test_cases = 1;
	scanf("%d\n", &test_cases);
	while (test_cases--) {
		run_testcase();
	}
}

/**
 * Main entry point
 */
int main() {
	run_test_loop();
	return 0;
}

