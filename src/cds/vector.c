#include <stdlib.h>
#include <string.h>

#include <cds/vector.h>

struct cds_vector_i {
    size_t size;
    size_t reserved;
    size_t type;

    size_t mod;

    struct cds_memory memory;
    uint8_t* data;
};

struct cds_vector_iterdata {
    size_t pos;
    size_t mod;
};

static int _cds_reserve(CDS_VECTOR(T) vector);
static int _cds_shrink(CDS_VECTOR(T) vector);

static CDS_ITER(T) _cds_iter_create(CDS_VECTOR(T) vector, struct cds_vector_iterdata data, bool reverse);
static bool _cds_iter_hasnext(void* structure, void** data);
static void* _cds_iter_next(void* structure, void** data);
static bool _cds_iter_hasback(void* structure, void** data);
static void* _cds_iter_back(void* structure, void** data);
static bool _cds_iter_similar(void* data, void* other);
static size_t _cds_iter_distance(void* data, void* other);
static bool _cds_iter_valid(void* structure, void* data);
static void _cds_iter_destroy(void* structure, void* data);

CDS_VECTOR(T) cds_vector_create(struct cds_vector_config config) {
    if (!cds_memory_valid(config.memory)) {
        return NULL;
    }

    struct cds_memory* memory = &config.memory;

    CDS_VECTOR(T) vector = memory->allocator(sizeof(struct cds_vector_i));

    if (vector != NULL) {
        vector->size = 0;
        vector->reserved = config.capacity;
        vector->type = config.type;

        vector->mod = 0;

        vector->memory = *memory;
        vector->data = memory->allocator(sizeof(uint8_t) * config.type * config.capacity);

        // no enough memory to create data
        if (vector->data == NULL) {
            memory->deallocator(vector);
            vector = NULL;
        }
    }

    return vector;
}

CDS_VECTOR(T) cds_vector_copy(CDS_VECTOR(T) vector, struct cds_memory memory) {
    // nothing to copy
    if (vector == NULL) {
        return NULL;
    }

    struct cds_vector_config config = {
        .type = vector->type,
        .capacity = vector->reserved,
        .memory = memory
    };
    CDS_VECTOR(T) other = cds_vector_create(config);

    if (other != NULL) {
        memcpy(other->data, vector->data, vector->type * vector->size);
    }

    return other;
}

CDS_VECTOR(T) cds_vector_from(CDS_VECTOR(T) vector, CDS_ITER(T) begin, CDS_ITER(T) end) {
    if (vector == NULL) {
        return NULL;
    }

    if (!cds_iter_valid(begin)) {
        begin = cds_vector_begin(vector);
    }
    if (!cds_iter_valid(end)) {
        end = cds_vector_end(vector);
    }

    struct cds_vector_config config = {
        .type = vector->type,
        .capacity = vector->size,
        .memory = vector->memory
    };
    CDS_VECTOR(T) other = cds_vector_create(config);

    if (other != NULL) {
        while (cds_iter_hasnext(begin) && !cds_iter_similar(begin, end)) {
            cds_vector_pushback(other, cds_iter_next(begin));
        }
    }

    return other;
}

void cds_vector_destroy(CDS_VECTOR(T) vector) {
    if (vector == NULL) {
        return;
    }

    cds_vector_clear(vector);

    cds_deallocator deallocator = vector->memory.deallocator;
    deallocator(vector->data);
    deallocator(vector);
}

int cds_vector_at(CDS_VECTOR(T) vector, size_t pos, void* out) {
    if (vector == NULL || pos >= vector->size) {
        return CDS_ERR;
    }

    memcpy(out, &vector->data[vector->type * pos], vector->type);
    return CDS_OK;
}

int cds_vector_front(CDS_VECTOR(T) vector, void* out) {
    return cds_vector_at(vector, 0, out);
}

int cds_vector_back(CDS_VECTOR(T) vector, void* out) {
    return cds_vector_at(vector, vector->size - 1, out);
}

CDS_ITER(T) cds_vector_begin(CDS_VECTOR(T) vector) {
    if (vector == NULL) {
        return NULL;
    }

    struct cds_vector_iterdata data = {.pos = 0};
    return _cds_iter_create(vector, data, false);
}

CDS_ITER(T) cds_vector_rbegin(CDS_VECTOR(T) vector) {
    if (vector == NULL) {
        return NULL;
    }

    struct cds_vector_iterdata data = {.pos = vector->size};
    return _cds_iter_create(vector, data, true);
}

CDS_ITER(T) cds_vector_end(CDS_VECTOR(T) vector) {
    if (vector == NULL) {
        return NULL;
    }

    struct cds_vector_iterdata data = {.pos = vector->size};
    return _cds_iter_create(vector, data, false);
}

CDS_ITER(T) cds_vector_rend(CDS_VECTOR(T) vector) {
    if (vector == NULL) {
        return NULL;
    }

    struct cds_vector_iterdata data = {.pos = vector->size};
    return _cds_iter_create(vector, data, true);
}

bool cds_vector_empty(CDS_VECTOR(T) vector) {
    return vector != NULL && vector->size == 0 ? true : false;
}

size_t cds_vector_size(CDS_VECTOR(T) vector) {
    return vector != NULL ? vector->size : 0;
}

int cds_vector_reserve(CDS_VECTOR(T) vector, size_t capacity) {
    if (vector == NULL) {
        return CDS_ERR;
    }

    if (vector->reserved >= capacity) {
        return CDS_OK;
    }

    cds_reallocator reallocator = vector->memory.reallocator;
    uint8_t* new_data = reallocator(vector->data, sizeof(uint8_t) * vector->type * capacity);

    if (new_data == NULL) {
        return CDS_ERR;
    }

    vector->reserved = capacity;
    vector->data = new_data;

    return CDS_OK;
}

size_t cds_vector_capacity(CDS_VECTOR(T) vector) {
    return vector != NULL ? vector->reserved : 0;
}

void cds_vector_shrink(CDS_VECTOR(T) vector) {
    if (vector == NULL || vector->size >= vector->reserved) {
        return;
    }

    cds_reallocator reallocator = vector->memory.reallocator;
    uint8_t* new_data = reallocator(vector->data, sizeof(uint8_t) * vector->type * vector->size);

    if (new_data == NULL) {
        return;
    }

    vector->reserved = vector->size;
    vector->data = new_data;
}

void cds_vector_clear(CDS_VECTOR(T) vector) {
    if (vector == NULL) {
        return;
    }

    vector->size = 0;
    vector->mod++;
}

int cds_vector_insert(CDS_VECTOR(T) vector, size_t pos, void* data) {
    if (vector == NULL || pos >= vector->size) {
        return CDS_ERR;
    }

    if (_cds_reserve(vector) != CDS_OK) {
        return CDS_ERR;
    }

    memmove(&vector->data[vector->type * (pos + 1)], &vector->data[vector->type * pos], vector->type * (vector->size - pos));
    memcpy(&vector->data[vector->type * pos], data, vector->type);

    vector->size++;
    vector->mod++;

    return CDS_OK;
}

int cds_vector_erase(CDS_VECTOR(T) vector, size_t pos) {
    if (vector == NULL || pos >= vector->size) {
        return CDS_ERR;
    }

    vector->size--;
    vector->mod++;

    if (vector->size > pos) {
        memmove(&vector->data[vector->type * pos], &vector->data[vector->type * (pos + 1)], vector->type * (vector->size - pos));
    }

    _cds_shrink(vector);

    return CDS_OK;
}

int cds_vector_pushback(CDS_VECTOR(T) vector, void* data) {
    if (vector == NULL) {
        return CDS_ERR;
    }

    if (_cds_reserve(vector) != CDS_OK) {
        return CDS_ERR;
    }

    memcpy(&vector->data[vector->type * vector->size], data, vector->type);

    vector->size++;
    vector->mod++;

    return CDS_OK;
}

int cds_vector_popback(CDS_VECTOR(T) vector, void* out) {
    if (vector == NULL || vector->size == 0) {
        return CDS_ERR;
    }

    vector->size--;
    vector->mod++;

    memcpy(out, &vector->data[vector->type * vector->size], vector->type);
    _cds_shrink(vector);

    return CDS_OK;
}

int cds_vector_resize(CDS_VECTOR(T) vector, size_t count, void* initializer) {
    if (vector == NULL) {
        return CDS_ERR;
    }

    if (count > vector->size) {
        if (cds_vector_reserve(vector, count) != CDS_OK) {
            return CDS_ERR;
        }

        for (size_t i = vector->size; i < count; i++) {
            cds_vector_pushback(vector, initializer);
        }
    } else if (count < vector->size) {
        for (size_t i = count; i < vector->size; i++) {
            cds_vector_erase(vector, i);
        }
    }

    return CDS_OK;
}

int cds_vector_swap(CDS_VECTOR(T) vector, CDS_VECTOR(T) other) {
    if (vector == NULL || other == NULL) {
        return CDS_ERR;
    }

    struct cds_vector_i swap = *vector;
    *vector = *other;
    *other = swap;

    vector->mod++;
    other->mod++;

    return CDS_OK;
}

static int _cds_reserve(CDS_VECTOR(T) vector) {
    size_t size = vector->size;
    if (vector->reserved > size) {
        return CDS_OK;
    }

    int (*reserver)(cds_vector, size_t) = cds_vector_reserve;
    return size > 0 ? reserver(vector, size * 2) : reserver(vector, 8);
}

static int _cds_shrink(CDS_VECTOR(T) vector) {
    if (vector->size != 0 && vector->reserved / vector->size < 4) {
        return CDS_OK;
    }

    cds_reallocator reallocator = vector->memory.reallocator;

    size_t new_reserved = vector->size != 0 ? vector->reserved / 2 : 0;
    uint8_t* new_data = reallocator(vector->data, sizeof(uint8_t) * vector->type * new_reserved);

    if (new_data == NULL) {
        return CDS_ERR;
    }

    vector->reserved = new_reserved;
    vector->data = new_data;

    return CDS_OK;
}

static CDS_ITER(T) _cds_iter_create(CDS_VECTOR(T) vector, struct cds_vector_iterdata data, bool reverse) {
    struct cds_memory* memory = &vector->memory;
    struct cds_vector_iterdata* iterdata = memory->allocator(sizeof(struct cds_vector_iterdata));

    if (iterdata == NULL) {
        return NULL;
    }

    memcpy(iterdata, &data, sizeof(struct cds_vector_iterdata));
    iterdata->mod = vector->mod;

    struct cds_iter_config config = {
        .memory = *memory,
        .initial_data = iterdata,
        .has_next = reverse ? _cds_iter_hasback : _cds_iter_hasnext,
        .next = reverse ? _cds_iter_back : _cds_iter_next,
        .has_back = reverse ? _cds_iter_hasnext : _cds_iter_hasback,
        .back = reverse ? _cds_iter_next : _cds_iter_back,
        .is_similar = _cds_iter_similar,
        .distance = _cds_iter_distance,
        .is_valid = _cds_iter_valid,
        .destroy = _cds_iter_destroy
    };

    return cds_iter_create(vector, config);
}

static bool _cds_iter_hasnext(void* structure, void** data) {
    if (structure == NULL || data == NULL) {
        return false;
    }

    CDS_VECTOR(T) vector = structure;
    struct cds_vector_iterdata* iterdata = *data;

    if (iterdata == NULL || vector->mod != iterdata->mod) {
        return false;
    }

    return vector->size > iterdata->pos;
}

static void* _cds_iter_next(void* structure, void** data) {
    if (structure == NULL || data == NULL) {
        return NULL;
    }

    CDS_VECTOR(T) vector = structure;
    struct cds_vector_iterdata* iterdata = *data;

    if (iterdata == NULL || vector->mod != iterdata->mod || iterdata->pos >= vector->size) {
        return NULL;
    }

    return &vector->data[vector->type * iterdata->pos++];
}

static bool _cds_iter_hasback(void* structure, void** data) {
    if (structure == NULL || data == NULL) {
        return false;
    }

    CDS_VECTOR(T) vector = structure;
    struct cds_vector_iterdata* iterdata = *data;

    if (iterdata == NULL || vector->mod != iterdata->mod) {
        return false;
    }

    return iterdata->pos > 0;
}

static void* _cds_iter_back(void* structure, void** data) {
    if (structure == NULL || data == NULL) {
        return NULL;
    }

    CDS_VECTOR(T) vector = structure;
    struct cds_vector_iterdata* iterdata = *data;

    if (iterdata == NULL || vector->mod != iterdata->mod || iterdata->pos == 0) {
        return NULL;
    }

    return &vector->data[vector->type * --iterdata->pos];
}

static bool _cds_iter_similar(void* data, void* other) {
    if (data == NULL || other == NULL) {
        return false;
    }

    struct cds_vector_iterdata* iterdata = data;
    struct cds_vector_iterdata* iterother = other;

    return iterdata->pos == iterother->pos;
}

static size_t _cds_iter_distance(void* data, void* other) {
    if (data == NULL || other == NULL) {
        return 0;
    }

    struct cds_vector_iterdata* iterdata = data;
    struct cds_vector_iterdata* iterother = other;

    return iterdata->pos > iterother->pos ? iterdata->pos - iterother->pos : iterother->pos - iterdata->pos;
}

static bool _cds_iter_valid(void* structure, void* data) {
    if (structure == NULL || data == NULL) {
        return false;
    }

    CDS_VECTOR(T) vector = structure;
    struct cds_vector_iterdata* iterdata = data;

    return vector->mod == iterdata->mod;
}

static void _cds_iter_destroy(void* structure, void* data) {
    if (structure == NULL) {
        return;
    }

    CDS_VECTOR(T) vector = structure;
    vector->memory.deallocator(data);
}

