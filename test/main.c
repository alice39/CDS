#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cds/vector.h>

#include <cds/graph.h>

int main() {
    
    CDS_VECTOR(int) vector = CDS_VECTOR_NEW(int);

    for (int i = 0; i < 4; i++) {
        cds_vector_pushback(vector, &i);
    }

    CDS_VECTOR_LOOP(vector, int*, i, {
        printf("ITER LOOP: %d\n", *i);
    });
    
    cds_vector_destroy(vector);

    /*
    //Create the new graph
    graph *m_g = cds_create_graph(5);

    // Adding edges
    cds_add_edge(m_g, 0, 1);
    cds_add_edge(m_g, 0, 2);
    cds_add_edge(m_g, 0, 3);
    cds_add_edge(m_g, 1, 2);
    cds_add_edge(m_g, 3, 1);


    cds_print_graph(m_g);

    cds_destroy_graph(m_g);

    /* Output: 
        Graph:
        0 -> 1
        0 -> 2
        0 -> 3
        1 -> 2
        3 -> 1 */
    
    return 0;
}
