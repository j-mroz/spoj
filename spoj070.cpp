#include <cstdio>
#include <vector>
#include <iostream>

// Works only with MSCS 10 + or gcc, clang or other compiler with __typeof
#if (_MSC_VER >= 1600) 
#define __typeof decltype
#endif
#define var(v, n) __typeof(n) v=(n)
#define for_each(i, c) for ( var(i, (c).begin()); i != (c).end(); ++i)
#define D(exp) std::cout << exp << std::endl

/**
 * Edge
 */
struct Edge {
    int dst;
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

    inline void CreateVertices(std::size_t u) {
        vertices_.resize(u);
        for (int i = 0; i < vertices_.size(); ++i)
            vertices_[i].sibling = i;
    }
    inline void CreateEdge(int u, int v, int type) {
        int further = std::max(u, v);
        if (further >= vertices_.size())
            vertices_.resize(further+1);
        if (type == 0) {
			std::swap(vertices_[v].sibling, vertices_[u].sibling);
            return;
        }
        Edge e = { v };
        EdgeListOf(u).push_back(e);
    }
    inline Vertex& VertexAt(int u) {
        return vertices_[u]; 
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
    std::vector<int> finish_times_;
    std::vector<int> depths_;
public:
    void ReadInput();
    void Run();
    void PrintOutput();
    void TopologicDfsVisit(int u);
	int MinSibling(int u);
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
		--u; --v;
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
    finish_times_.resize(graph_.VerticesCount(), -1);
    depths_.resize(graph_.VerticesCount(),  -1);

    for (int vertex = 0; vertex < graph_.VerticesCount(); ++vertex)
		if (depths_[vertex] == -1)
            TopologicDfsVisit(vertex);

    for (int vertex = 0; vertex < graph_.VerticesCount(); ++vertex) {
        result = std::max(result, depths_[vertex]);
        for_each(edge, graph_.EdgeListOf(vertex)) {
			int v = MinSibling(edge->dst);
            if (finish_times_[vertex] <= finish_times_[v]) {
                result = -1;
                return;
            }
        }
    }
}

/**
 * Toologicly orderging DFS visit
 */ 
void Algorithm::TopologicDfsVisit(int u) {
	depths_[u] = 0;
    // Visit all adjacent nodes
    for_each(edge, graph_.EdgeListOf(u)) {
        if (depths_[edge->dst] == -1)
            TopologicDfsVisit(edge->dst);
        depths_[u] = std::max(depths_[u], depths_[edge->dst]+1);
    }

	// Visit nodes on cyclic list representing "==" relation
	if (graph_.VertexAt(u).sibling != u) {
		if (depths_[graph_.VertexAt(u).sibling] == -1)
			TopologicDfsVisit(graph_.VertexAt(u).sibling);
		depths_[u] = std::max(depths_[u], depths_[graph_.VertexAt(u).sibling]);
	}

    finish_times_[u] = ++finish_timer_;
}

int Algorithm::MinSibling(int u) {
	int start = u;
	int result = u;
	while (graph_.VertexAt(u).sibling != start) {
		u = graph_.VertexAt(u).sibling;
		result = std::min(result, u);
	}
	return result;
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
