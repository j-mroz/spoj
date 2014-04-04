#include <cstdio>
#include <vector>
#include <iostream>

// Works only with MSCS 10 + or gcc, clang or other compiler with __typeof
#if (_MSC_VER >= 1600) 
#define __typeof decltype
#else 
#endif
#define var(v, n) __typeof(n) v=(n)
#define for_each(i, c) for ( var(i, (c).begin()); i != (c).end(); ++i)
#define D(exp) std::cout << exp << std::endl

/**
 * Edge
 */
struct Edge {
    int dst;
    int type;
};

/**
 * Vertex
 */
struct Vertex {
    std::vector<Edge> edges;
    int sibling;
};

/**
 * Adjacency listt representation of graph
 */
class AdjacencyList {
public:
    typedef std::vector<Edge> OutgoingEdgeList;
    typedef std::vector<Vertex> VertexOutgoingEdges;

    inline std::size_t CreateVertices(std::size_t u) {
        vertices_.resize(u);
        for_each(v, vertices_)
            v->sibling = -1;
        return vertices_.size();
    }
    inline void CreateEdge(int u, int v, int type) {
        int further = std::max(u, v);
        if (further >= vertices_.size())
            vertices_.resize(further+1);
        if (type == 0) {
            int last = u;
            while ()
        }
        Edge e = { v, type };
        EdgeListOf(u).push_back( e );
    }
    inline OutgoingEdgeList& EdgeListOf(int u) {
        return vertices_[u].edges; 
    }
    inline int VerticesCount() {
        return vertices_.size();
    }
private:
    VertexOutgoingEdges vertices_;
};


class Algorithm {
    AdjacencyList graph_;
    int finish_timer_;
    int result;
    std::vector<bool> visited_;
    std::vector<int> finish_times_;
    std::vector<int> depths_;
public:
    void ReadInput();
    void Run();
    void PrintOutput();
    int TopologicDfsVisit(int u, int edge_type);
};

/**
 * Read input parameters
 */ 
void Algorithm::ReadInput() {
    int vertices, edges;
    scanf("%d %d", &vertices, &edges);
    graph_.CreateVertices(vertices);
    for (int i = 0; i < vertices; ++i)
        graph_.EdgeListOf(i).reserve(edges);
    while (edges--) {
        int u, v, relation;
        scanf("%d %d %d", &u, &v, &relation);
        if (relation == -1) // <
            graph_.CreateEdge(u, v, 1);
        else if (relation == 1) // >
            graph_.CreateEdge(v, u, 1);
        else // ==
            graph_.CreateEdge(v, u, 0);
    }
}

/**
 * Executes algorithm
 */ 
void Algorithm::Run() {
    finish_timer_ = 0;
    result = 0;
    finish_times_.resize( graph_.VerticesCount(), -1 );
    depths_.resize( graph_.VerticesCount(), 0 );
    visited_.resize( graph_.VerticesCount(), false );

    for (int vertex = 0; vertex < graph_.VerticesCount(); ++vertex)
        if (!visited_[vertex])
            TopologicDfsVisit(vertex, 0);
    bool cycle_found = false;
    for (int vertex = 0; vertex < graph_.VerticesCount() && !cycle_found; ++vertex) {
        result = std::max(result, depths_[vertex]);
        for_each(edge, graph_.EdgeListOf(vertex)) {
            if (finish_times_[vertex] < finish_times_[edge->dst] && edge->type != 0) {
                cycle_found = true;
                result = -1;
                break;
            }
        }
    }
}

/**
 * Toologicly orderging DFS visit
 */ 
int Algorithm::TopologicDfsVisit(int u, int edge_type) {
    visited_[u] = true;
    for_each(edge, graph_.EdgeListOf(u)) {
        if (!visited_[edge->dst])
            TopologicDfsVisit(edge->dst, edge->type);
        depths_[u] = std::max(depths_[u], depths_[edge->dst]+edge->type);
    }
    finish_times_[u] = ++finish_timer_;
    return depths_[u];
}

/**
 * Prints result
 */ 
void Algorithm::PrintOutput() {
    if (result >= 0)
        printf("%d\n", result);
    else
        printf("NO\n");
}

/**
 * Main entry point
 */ 
int main(int argc, char* argv[]) {
    int test_number = 0;
    scanf("%d", &test_number);
    while (test_number--) {
        Algorithm algo;
        algo.ReadInput();
        algo.Run();
        algo.PrintOutput();
    }
    return 0;
}
