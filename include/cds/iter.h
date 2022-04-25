#ifndef CDS_ITER_GUARD_HEADER
#define CDS_ITER_GUARD_HEADER

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define CDS_ITER_LOOP(iter, type, var, block) \
    while (cds_iter_hasnext(iter)) {          \
        type var = cds_iter_next(iter);       \
        block                                 \
    }

typedef struct cds_iter_i* cds_iter;

struct cds_iter_config {
    void* initial_data;

    bool (*has_next)(void* structure, void** data);
    void* (*next)(void* structure, void** data);

    bool (*has_back)(void* structure, void** data);
    void* (*back)(void* structure, void** data);

    bool (*is_similar)(void* data, void* other);
    size_t (*distance)(void* data, void* other);

    bool (*is_valid)(void* structure, void* data);
    void (*destroy)(void* data);
};

cds_iter cds_iter_create(void* structure, struct cds_iter_config config);
bool cds_iter_valid(cds_iter iter);
void cds_iter_destroy(cds_iter iter);

bool cds_iter_similar(cds_iter first, cds_iter second);
size_t cds_iter_distance(cds_iter first, cds_iter second);

bool cds_iter_hasnext(cds_iter iter);
void* cds_iter_next(cds_iter iter);

bool cds_iter_hasback(cds_iter iter);
void* cds_iter_back(cds_iter iter);

#endif // CDS_ITER_GUARD_HEADER
