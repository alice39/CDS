#ifndef CDS_ITER_GUARD_HEADER
#define CDS_ITER_GUARD_HEADER

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <cds/cds.h>

#define CDS_ITER(type) cds_iter

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

CDS_ITER(T) cds_iter_create(void* structure, struct cds_iter_config config);
bool cds_iter_valid(CDS_ITER(T) iter);
void cds_iter_destroy(CDS_ITER(T) iter);

bool cds_iter_similar(CDS_ITER(T) first, CDS_ITER(T) second);
size_t cds_iter_distance(CDS_ITER(T) first, CDS_ITER(T) second);

bool cds_iter_hasnext(CDS_ITER(T) iter);
CDS_OBJ(T) cds_iter_next(CDS_ITER(T) iter);

bool cds_iter_hasback(CDS_ITER(T) iter);
CDS_OBJ(T) cds_iter_back(CDS_ITER(T) iter);

#endif // CDS_ITER_GUARD_HEADER
