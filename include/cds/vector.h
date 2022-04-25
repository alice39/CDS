#ifndef CDS_VECTOR_GUARD_HEADER
#define CDS_VECTOR_GUARD_HEADER

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "cds.h"
#include "iter.h"

/**
 * Vector with a type.
 *
 * It's used to indicate vector element type in syntax.
 *
 * @param type element type
 * @since 1.0
 */
#define CDS_VECTOR(type) cds_vector

/**
 * Loop vector with iterators.
 *
 * @param vector to iterate in
 * @param type element type
 * @param var variable for element
 * @param block function/lambda style
 * @since 1.0
 */
#define CDS_VECTOR_LOOP(vector, type, var, block) {                     \
    cds_iter _vector_iter_2022042512330000_ = cds_vector_begin(vector); \
    CDS_ITER_LOOP(_vector_iter_2022042512330000_, type, var, block)     \
    cds_iter_destroy(_vector_iter_2022042512330000_);                   \
}

/**
 * Vector struct pointer.
 *
 * @since 1.0
 */
typedef struct cds_vector_i* cds_vector;

/**
 * Configuration for vectors.
 *
 * @since 1.0
 */
struct cds_vector_config {
    // size of element to allocate
    size_t type;
    // initial capacity to reserve
    size_t capacity;
    // copy allocator for elements
    cds_callocator callocator;
    // destroyer for elements
    cds_destroyer destroyer;
};

// Constructor/Descontructor
/**
 * Create a new vector from configuration.
 *
 * @param config configuration to generate vector
 * @since 1.0
 * @return new vector or NULL if could not be created
 */
cds_vector cds_vector_create(struct cds_vector_config config);
/**
 * Create a new vector from an axisting vector.
 *
 * @param vector to be copied
 * @param callocator new copy allocator to new vector
 * @param destroyer new destroyer to new vector
 * @since 1.0
 * @return new vector or NULL if could not be created
 */
cds_vector cds_vector_copy(cds_vector vector, cds_callocator callocator, cds_destroyer destroyer);
/**
 * Create a new vector from another vector in a given range.
 * 
 * If begin is not valid, then it'll vector begin will be used and likewise to
 * end, if both are not valid, it'll be similar to use cds_vector_copy function.
 *
 * @param vector to slice on
 * @param begin where slice begins
 * @param end where slice ends
 * @since 1.0
 * @return new vector or NULL if could not be created
 */
cds_vector cds_vector_from(cds_vector vector, cds_iter begin, cds_iter end);
/**
 * Destroy a vector.
 * 
 * After this operation, vector should not be used anymore until be created
 * again.
 *
 * @param vector to be freed/destroyed
 * @since 1.0
 */
void cds_vector_destroy(cds_vector vector);

// Element Access
/**
 * Copy an element from vector in given position.
 * 
 * If these conditions are not met, then it'll fail:
 *  - vector should be a valid value
 *  - pos should be in range [0, size)
 *  - out should be a valid pointer
 *  - out should have enough memory to keep data
 * 
 * @param vector to look in
 * @param pos position to take
 * @param out output element
 * @since 1.0
 * @return CDS_OK if it was success otherwise CDS_ERR
 */
int cds_vector_at(cds_vector vector, size_t pos, void* out);
/**
 * Copy front/first element from vector.
 *
 * If these conditions are not met, then it'll fail:
 *  - vector should be a valid value
 *  - vector should have at least at element
 *  - out sohuld be a valid pointer
 *  - out should have enough memory to keep data
 *
 * @param vector to look in
 * @param out output element
 * @since 1.0
 * @return CDS_OK if it was success otherwise CDS_ERR
 */
int cds_vector_front(cds_vector vector, void* out);
/**
 * Copy back/last element from vector.
 *
 * If these conditions are not met, then it'll fail:
 *  - vector should be a valid value
 *  - vector should have at least at element
 *  - out sohuld be a valid pointer
 *  - out should have enough memory to keep data
 *
 * @param vector to look in
 * @param out output element
 * @since 1.0
 * @return CDS_OK if it was success otherwise CDS_ERR
 */
int cds_vector_back(cds_vector vector, void* out);

// iterators
/**
 * Create a new iterator for this vector from beginning.
 *
 * @param vector to create iterator from
 * @since 1.0
 * @return iterator or NULL if could not be created
 */
cds_iter cds_vector_begin(cds_vector vector);
/**
 * Create a new iterator for this vector from beginning in reverse mode.
 *
 * @param vector to create iterator from
 * @since 1.0
 * @return iterator or NULL if could not be created
 */
cds_iter cds_vector_rbegin(cds_vector vector);
/**
 * Create a new iterator for this vector from ending.
 *
 * @param vector to create iterator from
 * @since 1.0
 * @return iterator or NULL if could not be created
 */
cds_iter cds_vector_end(cds_vector vector);
/**
 * Create a new iterator for this vector from ending in reverse mode.
 *
 * @param vector to create iterator from
 * @since 1.0
 * @return iterator or NULL if could not be created
 */
cds_iter cds_vector_rend(cds_vector vector);

// Capacity Operators
/**
 * Check if vector is empty.
 *
 * @param vector to check emptiness
 * @since 1.0
 * @return true if empty otherwise false
 */
bool cds_vector_empty(cds_vector vector);
/**
 * Check vector used size.
 *
 * @param vector to check size
 * @since 1.0
 * @return size of vector
 */
size_t cds_vector_size(cds_vector vector);
/**
 * Reserve given capacity in vector.
 *
 * Take in count that capacity is not (the actual capacity) + (capacity), but
 * (the actual capacity) = (capacity) if capacity is greater than actual one
 *
 * @param vector to increase capacity
 * @param capacity how much to reserve
 * @since 1.0
 * @return CDS_OK if it could reverse said capacity otherwise CDS_ERR
 */
int cds_vector_reserve(cds_vector vector, size_t capacity);
/**
 * Check vector reserved size.
 *
 * @param vector to check capacity
 * @since 1.0
 * @return reserved capacity in vector
 */
size_t cds_vector_capacity(cds_vector vector);
/**
 * Shrink vector to fit.
 *
 * This will reduce reserved size until used size.
 *
 * @param vector to be shrinked
 * @since 1.0
 */
void cds_vector_shrink(cds_vector vector);

// Modifify Operators
/**
 * Erase all elements in vector.
 *
 * @param vector to clear
 * @since 1.0
 */
void cds_vector_clear(cds_vector vector);
/**
 * Insert a foward element in given position.
 *
 * Inserting an element will do that given position shifts 1 element, if there
 * is no element still in vector, this will fail.
 * 
 * @param vector to insert element in
 * @param pos position to take
 * @param data to be copied
 * @since 1.0
 * @return CDS_OK if it could insert it otherwise CDS_ERR
 */
int cds_vector_insert(cds_vector vector, size_t pos, void* data);
/**
 * Erase an element in given position.
 *
 * Position should be in range [0, size) otherwise it will fail.
 *
 * @param vector to erase element in
 * @param pos position to take out
 * @since 1.0
 * @return CDS_OK if it could erase it otherwise CDS_ERR
 */
int cds_vector_erase(cds_vector vector, size_t pos);
/**
 * Push back an element to vector.
 *
 * Pushing back an element can fail if:
 *  - vector is NULL
 *  - vector could not reserve enough space
 *  - data is NULL
 *
 * @param vector to push back element in
 * @param data to be copied
 * @since 1.0
 * @return CDS_OK if it could be pushed back otherwise CDS_ERR
 */
int cds_vector_pushback(cds_vector vector, void* data);
/**
 * Pop back an element from vector.
 *
 * If vector is empty, then it will fail.
 *
 * @param vector to pop back element in
 * @param out output popped element
 * @since 1.0
 * @return CDS_OK if it could pop element otherwise CDS_ERR
 */
int cds_vector_popback(cds_vector vector, void* out);
/**
 * Resize used size in vector.
 *
 * If count parameter is less than used size, then initializer can be
 * NULL safely
 *
 * @param vector to resize
 * @param count new size to take
 * @param initializer a default element to copy
 * @since 1.0
 * @return CDS_OK if it could be resized otherwise CDS_ERR
 */
int cds_vector_resize(cds_vector vector, size_t count, void* initializer);
/**
 * Swap two vector's elements to each other.
 *
 * If some of vectors are NULL, it will fail.
 *
 * @param vector first vector
 * @param other second vector
 * @since 1.0
 * @return CDS_OK if it could swap elements otherwise CDS_ERR
 */
int cds_vector_swap(cds_vector vector, cds_vector other);

#endif // CDS_VECTOR_GUARD_HEADER
