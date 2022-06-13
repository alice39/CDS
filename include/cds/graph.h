#ifndef CDS_GRAPH_HEADER 
#define CDS_GRAPH_HEADER

#include <stdbool.h>

typedef struct cds_graph cds_graph;


cds_graph *cds_create_graph(int nodes);

void cds_destroy_graph(cds_graph* g);

void cds_print_graph(cds_graph* g);

bool cds_add_edge(cds_graph* g, unsigned int from_node, unsigned int to_node);

bool cds_has_edge(cds_graph* g, unsigned int from_node, unsigned int to_node);

#endif 