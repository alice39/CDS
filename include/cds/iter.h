#ifndef CDS_ITER_GUARD_HEADER
#define CDS_ITER_GUARD_HEADER

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <cds/cds.h>

/**
 * Iterator with a type.
 *
 * It's used to indicate iterator element type in syntax.
 *
 * @param type element type
 * @since 1.0
 */
#define CDS_ITER(type) cds_iter

/**
 * Iterator loop until there'll be no elements.
 *
 * @param iter iterator to loop
 * @param type iterator element type
 * @param var element name in loop scope
 * @param block function/lambda style to execute
 * @since 1.0
 */
#define CDS_ITER_LOOP(iter, type, var, block) \
    while (cds_iter_hasnext(iter)) {          \
        type var = cds_iter_next(iter);       \
        block                                 \
    }

/**
 * Iterator struct pointer.
 *
 * @since 1.0
 */
typedef struct cds_iter_i* cds_iter;

/**
 * Configuration for iterators.
 *
 * @since 1.0
 */
struct cds_iter_config {
    struct cds_memory memory;
    void* initial_data;

    bool (*has_next)(void* structure, void** data);
    void* (*next)(void* structure, void** data);

    bool (*has_back)(void* structure, void** data);
    void* (*back)(void* structure, void** data);

    bool (*is_similar)(void* data, void* other);
    size_t (*distance)(void* data, void* other);

    bool (*is_valid)(void* structure, void* data);
    void (*destroy)(void* structure, void* data);
};

/**
 * Create a new iterator from structure and configuration.
 *
 * @param structure where iterator comes from
 * @param config configuration to generate iterator
 * @since 1.0
 * @return new iterator or NULL if could not be created
 */
CDS_ITER(T) cds_iter_create(void* structure, struct cds_iter_config config);
/**
 * Check if iterator still is valid.
 *
 * @param iter to check out
 * @since 1.0
 * @return true if it's still valid otherwise false
 */
bool cds_iter_valid(CDS_ITER(T) iter);
/**
 * Destroy an iterator.
 *
 * @param iter to be freed/destroyed
 * @since 1.0
 */
void cds_iter_destroy(CDS_ITER(T) iter);

/**
 * Compare two iterators to check if they're similar.
 * 
 * @param first iterator to compare with
 * @param second iterator to be compared
 * @since 1.0
 * @return true if both are similar otherwise false
 */
bool cds_iter_similar(CDS_ITER(T) first, CDS_ITER(T) second);
/**
 * Calculate the distance between these two iterators.
 *
 * @param first iterator to distance with
 * @param second iterator to be calculated with
 * @since 1.0
 * @return the distance between both iterators
 */
size_t cds_iter_distance(CDS_ITER(T) first, CDS_ITER(T) second);

/**
 * Check if iterator has more elements to be scanned.
 *
 * @param iter iterator to check
 * @since 1.0
 * @return true if there is next otherwise false
 */
bool cds_iter_hasnext(CDS_ITER(T) iter);
/**
 * Fetch for next element in iterator.
 *
 * If there is no more element to fetch, it will return NULL.
 * 
 * @see cds_iter_hasnext
 * @since 1.0
 * @return pointer to element or NULL if there is no
 */
CDS_OBJ(T) cds_iter_next(CDS_ITER(T) iter);

/**
 * Check if iterator has more elements to be scanned in reverse.
 *
 * @param iter iterator to check
 * @since 1.0
 * @return true if there is previous otherwise false
 */
bool cds_iter_hasback(CDS_ITER(T) iter);
/**
 * Fetch for previous element in iterator.
 *
 * If there is no more element to fetch, it will return NULL.
 * 
 * @see cds_iter_hasback
 * @since 1.0
 * @return pointer to element or NULL if there is no
 */
CDS_OBJ(T) cds_iter_back(CDS_ITER(T) iter);

#endif // CDS_ITER_GUARD_HEADER
