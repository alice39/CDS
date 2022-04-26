#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cds/vector.h>

int main() {
    CDS_VECTOR(int) vector = CDS_VECTOR_NEW(int);

    for (int i = 0; i < 4; i++) {
        cds_vector_pushback(vector, &i);
    }

    CDS_VECTOR_LOOP(vector, int*, i, {
        printf("ITER LOOP: %d\n", *i);
    });

    cds_vector_destroy(vector);

    return 0;
}
