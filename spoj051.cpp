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

namespace impl {
const int WHITE = -1;
const int GRAY = 0;
const int BLACK = 1;
} // impl

/**
 * Dfs Visitor base class, fuse with static polymorphism
 */
template <typename Graph>
struct DfsVisitorBase {
    inline void OnVertexStart(Graph& g, std::size_t u) { }
    inline void OnVertexFinish(Graph& g, std::size_t u) { }
    inline void OnTreeEdge(Graph& g, typename Graph::Edge& e) { }
    inline void OnBackEdge(Graph& g, typename Graph::Edge& e) { }
    inline void OnCrossOrForwardEdge(Graph& g, typename Graph::Edge& e) { }
    inline void OnTreeFinish(Graph& g, std::size_t root) { }
};

/**
 * Dfs Visit function
 */
template <typename Graph, typename Visitor, typename ColorMap>
void DfsVisit(Graph& graph, Visitor& visitor, ColorMap& colors, std::size_t u_index) {
    visitor.OnVertexStart(graph, u_index);
    colors.at(u_index) = impl::GRAY;

    for_outedge(edge_iterator, graph.EdgeListOf(u_index)) {
        typename Graph::Edge edge(*edge_iterator, u_index);

        if (colors.at(edge_iterator->dst) == impl::WHITE) {
            visitor.OnTreeEdge(graph, edge);
            //DfsVisit(graph, visitor, colors, edge_iterator->dst);
        }
        else if (colors.at(edge_iterator->dst) == impl::GRAY) {
            visitor.OnBackEdge(graph, edge);
        }
        else {
            visitor.OnCrossOrForwardEdge(graph, edge);
        }
    }

    colors.at(u_index) = impl::BLACK;
    visitor.OnVertexFinish(graph, u_index);
}

/**
 * DFS algorithm
 */
template <typename Graph, typename Visitor>
void DeepFirstSearch(Graph& graph, Visitor& visitor) {
    std::vector<int> colors(graph.VerticesCount(), impl::WHITE);
    for (std::size_t vertex_index = 0; vertex_index < graph.VerticesCount(); ++vertex_index)
        if (colors.at(vertex_index) == impl::WHITE) {
            DfsVisit(graph, visitor, colors, vertex_index);
            visitor.OnTreeFinish(graph, vertex_index);
        }
}

/**
 * DFS algorithm with specified order of vertices
 */
template <typename Graph, typename Visitor>
void DeepFirstSearch(Graph& graph, Visitor& visitor, std::vector<std::size_t> vertices_order) {
    if (vertices_order.size() != graph.VerticesCount())
        throw std::range_error("vertices_order size does not mach vertex count");

    std::vector<int> colors(graph.VerticesCount(), impl::WHITE);

    for (std::size_t index = 0; index < vertices_order.size(); ++index) {
        if (colors.at(vertices_order.at(index)) == impl::WHITE) {
            DfsVisit(graph, visitor, colors, vertices_order.at(index));
        }
        visitor.OnTreeFinish(graph, vertices_order.at(index));
    }
}


template <typename Graph>
struct VertexFinishOrderVisitor : public DfsVisitorBase<Graph> {
    inline void OnVertexFinish(Graph& g, std::size_t u) {
        visit_order.push_back(u);
    }
    std::vector<std::size_t> visit_order;
};

template <typename Graph>
struct SccVisitor : public DfsVisitorBase<Graph> {
    SccVisitor() {
        finished = false;
        tree_count = 0;
    }
    inline void OnVertexStart(Graph& g, std::size_t u) {
        if (!finished)
            ++tree_count;
    }
    inline void OnTreeFinish(Graph& g, std::size_t root) {
        finished = true;
    }
    bool finished;
    std::size_t tree_count;
};


/**
 * Strongly connected components algorithm
 */
template <typename Graph, typename Visitor>
void StronglyConnectedComponents(Graph& graph, Visitor& visitor) {
    // DFS
    VertexFinishOrderVisitor<Graph> scc_visitor;
    DeepFirstSearch(graph, scc_visitor);

    // DFS on transposed graph with reversed order (based on 1st DFS visit time)
    std::reverse(scc_visitor.visit_order.begin(), scc_visitor.visit_order.end());
    Graph transposed_graph;
    Transpose(graph, transposed_graph);
    //for (int i = 0; i < graph.VerticesCount(); ++i)
    DeepFirstSearch(transposed_graph, visitor, scc_visitor.visit_order);
}

class Algorithm {
    typedef AdjacencyList Graph;
    Graph graph_;
    std::size_t result;
public:
    void ReadInput();
    void Run();
    void PrintOutput();
} algo;

/**
 * Runs the algorithm - based on BFS
 */
void Algorithm::Run() {
    SccVisitor<Graph> scc_visitor;
    StronglyConnectedComponents(graph_, scc_visitor);
    result = scc_visitor.tree_count;
}

/**
 * Parse input into Algorithm
 */
void Algorithm::ReadInput() {
    int participants_count;
    scanf("%d", &participants_count);
    graph_.CreateVertices(participants_count);
    for (int participant = 1; participant <= participants_count; ++participant) {
        int losing_count;
        scanf("%d", &losing_count);
        for (int looser = 1; looser <= losing_count; ++looser) {
            int looser_id;
            scanf("%d", &looser_id);
            graph_.CreateEdge(participant-1, looser_id-1);
        }
    }
}

/**
 * Prints the output
 */
void Algorithm::PrintOutput() {
    printf("%ld\n", result);
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














