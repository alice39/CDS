#ifndef CDS_GRAPH_HEADER 
#define CDS_GRAPH_HEADER

#include <stdbool.h>

typedef struct graph graph;


graph *create_graph(int nodes);

void destroy_graph(graph* g);

void print_graph(graph* g);

bool add_edge(graph* g, unsigned int from_node, unsigned int to_node);

bool has_edge(graph* g, unsigned int from_node, unsigned int to_node);

#endif 