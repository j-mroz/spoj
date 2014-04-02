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

    inline std::size_t PushVertex() {
        vertex_edges_.push_back(OutgoingEdgeList());
        return vertex_edges_.size()-1;
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
    inline void OnTreeStart(Graph& g, std::size_t root) { }
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
            DfsVisit(graph, visitor, colors, edge_iterator->dst);
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
            visitor.OnTreeStart(graph, vertex_index);
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
            visitor.OnTreeStart(graph, vertices_order.at(index));
            DfsVisit(graph, visitor, colors, vertices_order.at(index));
            visitor.OnTreeFinish(graph, vertices_order.at(index));
        }
    }
}

#define D(exp) std::cout << exp << std::endl
template <typename Graph>
struct VertexFinishOrderVisitor : public DfsVisitorBase<Graph> {
    inline void OnVertexFinish(Graph& g, std::size_t u) {
        visit_order.push_back(u);
    }
    std::vector<std::size_t> visit_order;
};

template <typename Graph>
struct SccVisitor : public DfsVisitorBase<Graph> {
    //std::vector<std::vector<std::size_t> > scc;
    std::vector<std::size_t> vertices_roots;
    std::vector<std::size_t> scc_sizes;
    std::size_t current_root;
    std::size_t current_scc_id;
    std::size_t current_scc_count;
    Graph scc_graph;
    SccVisitor(Graph& orig) {
        vertices_roots.resize(orig.VerticesCount());
        scc_sizes.resize(orig.VerticesCount());
        for (std::size_t i = 0; i < vertices_roots.size(); ++i)
            vertices_roots[i] = -1;
        current_root = -1;
        current_scc_id = -1;
        current_scc_count = 0;
    }
    inline void OnVertexStart(Graph& g, std::size_t u) {
        //scc.back().push_back(u);
        //vertices_roots[u] = current_root;
        vertices_roots[u] = current_scc_id;
        ++current_scc_count;
    }
    inline void OnTreeStart(Graph& g, std::size_t root) {
        //scc.push_back(std::vector<std::size_t>());
        current_root = root;
        current_scc_count = 0;
        ++current_scc_id;
        //D( current_scc_count );

        //tree_roots.push_back(root);
        scc_graph.PushVertex();
    }
    inline void OnTreeFinish(Graph& g, std::size_t root) {
        scc_sizes[vertices_roots.at(root)] = current_scc_count;
        // D( vertices_roots.at(root) );
        // D( root );
        // D( scc_sizes[vertices_roots.at(root)] );
    }
    inline void OnTreeEdge(Graph& g, typename Graph::Edge& e) { 
    }
    inline void OnCrossOrForwardEdge(Graph& g, typename Graph::Edge& e) {
        if (vertices_roots.at(e.dst) != vertices_roots.at(e.src))
            scc_graph.CreateEdge( vertices_roots.at(e.dst), vertices_roots.at(e.src) );
        // PrintEdge(e); 
        // D (vertices_roots.at(e.dst) << "," << vertices_roots.at(e.src));
    }
    inline void OnBackEdge(Graph& g, typename Graph::Edge& e) { 
        //PrintEdge(e); 

    }
    // inline void OnCrossOrForwardEdge(Graph& g, typename Graph::Edge& e) { PrintEdge(e); }
    inline void PrintEdge(typename Graph::Edge& e) { D( e.src+1 << "->" << e.dst+1 ); }
};



template <typename Graph>
struct DebugPrinter : public DfsVisitorBase<Graph> {
    inline void OnTreeEdge(Graph& g, typename Graph::Edge& e) { PrintEdge(e); }
    inline void OnBackEdge(Graph& g, typename Graph::Edge& e) { PrintEdge(e); }
    inline void OnCrossOrForwardEdge(Graph& g, typename Graph::Edge& e) { PrintEdge(e); }
    inline void PrintEdge(typename Graph::Edge& e) { D( e.src+1 << "->" << e.dst+1 ); }
};

/**
 * Strongly connected components algorithm
 */
template <typename Graph, typename Visitor>
void StronglyConnectedComponents(Graph& graph, Graph& transposed_graph,  Visitor& visitor) {
    // DFS
    VertexFinishOrderVisitor<Graph> scc_visitor;
    DeepFirstSearch(graph, scc_visitor);

    // DFS on transposed graph with reversed order (based on 1st DFS visit time)
    std::reverse(scc_visitor.visit_order.begin(), scc_visitor.visit_order.end());
    //Graph transposed_graph;
    //Transpose(graph, transposed_graph);
    DeepFirstSearch(transposed_graph, visitor, scc_visitor.visit_order);
}

class Algorithm {
    typedef AdjacencyList Graph;
    Graph graph_;
    Graph transposed_graph_;
    //Graph graph_tr_;
    std::size_t result;
public:
    void ReadInput();
    void Run();
    void PrintOutput();
};

/**
 * Runs the algorithm - based on BFS
 */
void Algorithm::Run() {
    SccVisitor<Graph> scc_visitor(graph_);
    StronglyConnectedComponents(graph_, transposed_graph_, scc_visitor);

    Graph transposed_scc_graph;
    Transpose(scc_visitor.scc_graph, transposed_scc_graph);
    result = 0;
    std::size_t noin_vertex_count = 0;
    //D( transposed_scc_graph.VerticesCount() );
    for (std::size_t i = 0; i < transposed_scc_graph.VerticesCount(); ++i) {
        if (transposed_scc_graph.EdgeListOf(i).size() == 0) {
            ++noin_vertex_count;
            result = scc_visitor.scc_sizes[i];
        }
        // D( scc_visitor.scc_sizes[i] );
        // D( transposed_scc_graph.EdgeListOf(i).size() );
    }
    // D( "noin_vertex_count=" << noin_vertex_count );
    // D( "result=" << result );
    if (noin_vertex_count > 1) {
        result = 0;
    }

    //DebugPrinter<Graph> dprinter;
    //DeepFirstSearch(transposed_scc_graph, dprinter);

    // for (int i = 0; i < scc_visitor.scc.size(); ++i) {
    //     for (int j = 0; j < scc_visitor.scc[i].size(); ++j)
    //         std::cout << scc_visitor.scc[i][j]+1 << ", ";
    //     std::cout << std::endl;
    // }
    // result = scc_visitor.tree_count;
}

/**
 * Parse input into Algorithm
 */
void Algorithm::ReadInput() {
    int participants_count;
    scanf("%d", &participants_count);
    graph_.CreateVertices(participants_count);
    for (int participant = 1; participant <= participants_count; ++participant) {
        int winner_count;
        scanf("%d", &winner_count);
        for (int winner = 1; winner <= winner_count; ++winner) {
            int winner_id;
            scanf("%d", &winner_id);
            graph_.CreateEdge(winner_id-1, participant-1);
            transposed_graph_.CreateEdge(participant-1, winner_id-1);
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
    Algorithm algo;
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


























