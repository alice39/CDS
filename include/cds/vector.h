#ifndef CDS_VECTOR_GUARD_HEADER
#define CDS_VECTOR_GUARD_HEADER

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "cds.h"
#include "iter.h"

#define CDS_VECTOR(type) cds_vector

#define CDS_VECTOR_LOOP(vector, type, var, block) {                     \
    cds_iter _vector_iter_2022042512330000_ = cds_vector_begin(vector); \
    CDS_ITER_LOOP(_vector_iter_2022042512330000_, type, var, block)     \
    cds_iter_destroy(_vector_iter_2022042512330000_);                   \
}

typedef struct cds_vector_i* cds_vector;

struct cds_vector_config {
    size_t type;
    size_t capacity;
    cds_callocator callocator;
    cds_destroyer destroyer;
};

cds_vector cds_vector_create(struct cds_vector_config config);
cds_vector cds_vector_copy(cds_vector vector, cds_callocator callocator, cds_destroyer destroyer);
void cds_vector_destroy(cds_vector vector);

// element access
int cds_vector_at(cds_vector vector, size_t pos, void* out);
int cds_vector_front(cds_vector vector, void* out);
int cds_vector_back(cds_vector vector, void* out);

// iterators
cds_iter cds_vector_begin(cds_vector vector);
cds_iter cds_vector_rbegin(cds_vector vector);
cds_iter cds_vector_end(cds_vector vector);
cds_iter cds_vector_rend(cds_vector vector);

// capacity operators
bool cds_vector_empty(cds_vector vector);
size_t cds_vector_size(cds_vector vector);
int cds_vector_reserve(cds_vector vector, size_t capacity);
size_t cds_vector_capacity(cds_vector vector);
void cds_vector_shrink(cds_vector vector);

// modifify operators
void cds_vector_clear(cds_vector vector);
int cds_vector_insert(cds_vector vector, size_t pos, void* data);
int cds_vector_erase(cds_vector vector, size_t pos);
int cds_vector_pushback(cds_vector vector, void* data);
int cds_vector_popback(cds_vector vector, void* out);
// if count is less, then initializer, allocator can be NULL
int cds_vector_resize(cds_vector vector, size_t count, void* initializer);
int cds_vector_swap(cds_vector vector, cds_vector other);

#endif // CDS_VECTOR_GUARD_HEADER
