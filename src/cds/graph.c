/* 
----------------------------------------------------------------
Graph representation in C 
- Using adjacency matrix (exists a lot of ways to implement a graph it depends 
on how you're going to use the graph, space, implementation with before code, etc...)

Why use a adjacency matrix?, well is really easy to use, to represent and change nodes... 
Just with the downside of space, when with have a graph with a lot of nodes but not many edges, the matrix will be really big.
----------------------------------------------------------------

Define:
    Graph: Set of objects (nodes or vertices) together with a set of edges or arcs.
        - Undirected: They edges are in a unordered pair of vertices, where all the edges are bidirectional.
        - Directed: Unlike the undirected, the directed graphs have edges ordered pair of vertices. also the edges have a direction.
        (Recommend read about graph theory)
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <cds/graph.h>


struct cds_graph {
    int nodes;
    bool **edges;
};


// Function to create a graph, with the number of nodes
cds_graph *cds_create_graph(int nodes) {
    // Assign the size memory for the graph
    cds_graph *g = malloc(sizeof(*g));

    // Check if g is null (maybe problems with the allocator)
    if (g == NULL){
        return NULL;
    }

    // Add nodes and edges
    g -> nodes = nodes;
    // Allocate the matrix
    g -> edges = calloc(sizeof(bool*), g->nodes);

    // Check if edges is null (just for check problems in memory)
    if (g->edges == NULL){
        free(g);
        return NULL;
    }


    for (int i=0; i < g->nodes; i++) {
        g->edges[i] = calloc(sizeof(bool), g->nodes);
        if (g->edges[i] == NULL){
            cds_destroy_graph(g);
            return NULL;
        }
    }

    return g; 
}

// Function to destroy a graph
void cds_destroy_graph(cds_graph* g) {
    // Check if the graph edges are NULL  
    if (g == NULL || g->edges == NULL) {
        return;
    }


    // Deleting all edges of the graph 
    for (int i=0; i < g->nodes; i++) {
        if  (g->edges[i] != NULL){
            free(g->edges[i]);
        }
    }

    free(g->edges);
    // Delete the graph pointer
    free(g);
}

// Function to display the graph in memory
void cds_print_graph(cds_graph* g) {
    printf("digraph {\n");

    // A for inside other to go through the matrix or array 2D 
    for (int from=0; from < g->nodes; from++) {
        for (int to=0; to < g->nodes; to++) {
            if (g->edges[from][to]) {
                printf("%d -> %d;\n", from, to);
            }
        }
    }
    printf("}\n");
}

// Function to add a edge to a graph
bool cds_add_edge(cds_graph *g, unsigned int from_node, unsigned int to_node) {
    // Check for possible bugs
    assert(g != NULL);
    assert(from_node < g->nodes);
    assert(to_node < g->nodes);

    // If the edge already exists, return false
    if (cds_has_edge(g, from_node, to_node)) {
        return false;
    }

    // Add the new edge
    g->edges[from_node][to_node] = true;
    return true;
}

bool cds_has_edge(cds_graph *g, unsigned int from_node, unsigned int to_node) {
    // Check for possible bugs
    assert(g != NULL);
    assert(from_node < g->nodes);
    assert(to_node < g->nodes);

    // return the edge if exists
    return g->edges[from_node][to_node];
}
