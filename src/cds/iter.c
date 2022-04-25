#include <stdlib.h>

#include <cds/iter.h>

struct cds_iter_i {
    void* structure;
    void* data;

    bool (*has_next)(void* structure, void** data);
    void* (*next)(void* structure, void** data);

    bool (*has_back)(void* structure, void** data);
    void* (*back)(void* structure, void** data);
    
    bool (*is_similar)(void* data, void* other);
    size_t (*distance)(void* data, void* other);

    bool (*is_valid)(void* structure, void* data);
    void (*destroy)(void* data);
};

cds_iter cds_iter_create(void* structure, struct cds_iter_config config) {
    if (structure == NULL) {
        return NULL;
    }

    cds_iter iter = malloc(sizeof(struct cds_iter_i));
    
    if (iter != NULL) {
        iter->structure = structure;
        iter->data = config.initial_data;

        iter->has_next = config.has_next;
        iter->next = config.next;

        iter->has_back = config.has_back;
        iter->back = config.back;

        iter->is_similar = config.is_similar;
        iter->distance = config.distance;

        iter->is_valid = config.is_valid;
        iter->destroy = config.destroy;
    }

    return iter;
}

bool cds_iter_valid(cds_iter iter) {
    if (iter == NULL | iter->is_valid == NULL) {
        return false;
    }

    return iter->is_valid(iter->structure, iter->data);
}

void cds_iter_destroy(cds_iter iter) {
    if (iter == NULL) {
        return;
    }

    if (iter->destroy != NULL) {
        iter->destroy(iter->data);
    }

    free(iter);
}

bool cds_iter_similar(cds_iter first, cds_iter second) {
    if (first == NULL || first->is_similar == NULL || second == NULL) {
        return false;
    }

    return first->is_similar(first->data, second->data) ? true : false;
}

size_t cds_iter_distance(cds_iter first, cds_iter second) {
    if (first == NULL || first->distance == NULL || second == NULL) {
        return 0;
    }

    return first->distance(first->data, second->data);
}

bool cds_iter_hasnext(cds_iter iter) {
    if (iter == NULL || iter->has_next == NULL) {
        return false;
    }

    return iter->has_next(iter->structure, &iter->data) ? true : false;
}

void* cds_iter_next(cds_iter iter) {
    if (iter == NULL || iter->next == NULL) {
        return NULL;
    }

    return iter->next(iter->structure, &iter->data);
}

bool cds_iter_hasback(cds_iter iter) {
    if (iter == NULL || iter->has_back == NULL) {
        return false;
    }

    return iter->has_back(iter->structure, &iter->data);
}

void* cds_iter_back(cds_iter iter) {
    if (iter == NULL || iter->back == NULL) {
        return NULL;
    }

    return iter->back(iter->structure, &iter->data);
}

