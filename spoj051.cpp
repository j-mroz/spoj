#include <cstdio>
#include <iostream>
#include <utility>
#include <vector>
#include <list>
#include <algorithm>
#include <stdexcept>
#include <iterator>

#define for_outedge(e, coll) \
    for (typename Graph::OutgoingEdgeList::iterator e = coll.begin(); edge_iterator != coll.end(); ++e)

/**
 * OutEdge
 */
struct OutEdge {
    std::size_t dst;
};


/**
 * Edge, Edge is OutEdge too
 */
struct Edge : OutEdge {
    Edge(const OutEdge out_edge, std::size_t src) {
        this->src = src;
        this->dst = out_edge.dst;
    }
    std::size_t src;
};


/**
 * Adjacency listt representation of graph
 */
class AdjacencyList {
public:
    typedef ::OutEdge OutEdge;
    typedef ::Edge Edge;
    typedef std::list<OutEdge> OutgoingEdgeList;
    typedef std::vector<OutgoingEdgeList> VertexOutgoingEdges;

    inline std::size_t CreateVertices(std::size_t u) {
        vertex_edges_.resize(u);
        return vertex_edges_.size();
    }

    inline void CreateEdge(std::size_t u, std::size_t v) {
        std::size_t further = std::max(u, v);
        if (further >= vertex_edges_.size())
            vertex_edges_.resize(further+1);
        OutEdge e = {v};
        EdgeListOf(u).push_back(e);
    }

    inline OutgoingEdgeList& EdgeListOf(std::size_t u) {
        return vertex_edges_.at(u); 
    }

    inline std::size_t VerticesCount() {
        return vertex_edges_.size();
    }

    template<typename Graph>
    friend void Transpose(Graph& oryginal, Graph& transposed);

// Data
private:
    VertexOutgoingEdges vertex_edges_;
};

/**
 * Transpose a graph
 */
template<typename Graph>
void Transpose(Graph& oryginal, Graph& transposed) {
    transposed.vertex_edges_.resize(oryginal.vertex_edges_.size());
    for (std::size_t vertex_index = 0; vertex_index < oryginal.vertex_edges_.size(); ++vertex_index) {
        for_outedge(edge_iterator, oryginal.EdgeListOf(vertex_index)) {
            typename Graph::OutEdge transposed_edge = *edge_iterator;
            transposed_edge.dst = vertex_index;
            transposed.EdgeListOf(edge_iterator->dst).push_back(transposed_edge);
        }
    }
}




class Algorithm {
    typedef AdjacencyList Graph;
    Graph graph_;
public:
    void ReadInput();
    void Run();
    void PrintOutput();
} algo;


/**
 * Runs the algorithm - based on BFS
 */
void Algorithm::Run() {

}


/**
 * Parse input into Algorithm
 */
void Algorithm::ReadInput() {
    int participants_count;
    scanf("%d", &participants_count);
    graph_.CreateVertices(participants_count);
    for (int participant = 0; participant <= participants_count; ++participant) {
        int losing_count;
        scanf("%d", &losing_count);
        for (int looser = 1; looser <= losing_count; ++looser) {
            int looser_id;
            scanf("%d", &looser_id);
            graph_.CreateEdge(participant, looser_id);
        }
    }

}

/**
 * Prints the output
 */
void Algorithm::PrintOutput() {
}

/**
 * Run single test case
 */
void run_testcase() {
    algo.ReadInput();
    algo.Run();
    algo.PrintOutput();
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