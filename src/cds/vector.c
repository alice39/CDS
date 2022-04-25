#include <stdlib.h>
#include <string.h>

#include <cds/vector.h>

struct cds_vector_i {
    size_t size;
    size_t reserved;
    size_t type;

    size_t mod;

    cds_callocator callocator;
    cds_destroyer destroyer;
    uint8_t* data;
};

struct cds_vector_iterdata {
    size_t pos;
    size_t mod;
};

static int _cds_reserve(cds_vector vector);
static int _cds_shrink(cds_vector vector);

static cds_iter _cds_iter_create(cds_vector vector, struct cds_vector_iterdata data, bool reverse);
static bool _cds_iter_hasnext(void* structure, void** data);
static void* _cds_iter_next(void* structure, void** data);
static bool _cds_iter_hasback(void* structure, void** data);
static void* _cds_iter_back(void* structure, void** data);
static bool _cds_iter_valid(void* structure, void* data);
static void _cds_iter_destroy(void* data);

cds_vector cds_vector_create(struct cds_vector_config config) {
    cds_vector vector = malloc(sizeof(struct cds_vector_i));

    if (config.callocator == NULL) {
        config.callocator = cds_simple_callocator;
    }
    if (config.destroyer == NULL) {
        config.destroyer = cds_simple_destroyer;
    }

    if (vector != NULL) {
        vector->size = 0;
        vector->reserved = config.capacity;
        vector->type = config.type;

        vector->mod = 0;

        vector->callocator = config.callocator;
        vector->destroyer = config.destroyer;
        vector->data = malloc(sizeof(uint8_t) * config.type * config.capacity);

        // no enough memory to create data
        if (vector->data == NULL) {
            free(vector);
            vector = NULL;
        }
    }

    return vector;
}

cds_vector cds_vector_copy(cds_vector vector, cds_callocator callocator, cds_destroyer destroyer) {
    // nothing to copy
    if (vector == NULL) {
        return NULL;
    }

    struct cds_vector_config config = {
        .type = vector->type,
        .capacity = vector->reserved,
        .callocator = callocator,
        .destroyer = destroyer
    };
    cds_vector other = cds_vector_create(config);

    if (other != NULL) {
        for (size_t i = 0; i < vector->size; i++) {
            size_t data_index = i * vector->type;

            void* element = other->callocator(&vector->data[data_index], vector->type);
            memcpy(&other->data[data_index], element, vector->type);

            // destroyer should not called
            // because element was copied into data
            free(element);
        }
    }

    return other;
}

void cds_vector_destroy(cds_vector vector) {
    if (vector == NULL) {
        return;
    }

    cds_vector_clear(vector);
    free(vector->data);
    free(vector);
}

int cds_vector_at(cds_vector vector, size_t pos, void* out) {
    if (vector == NULL || pos >= vector->size) {
        return CDS_ERR;
    }

    memcpy(out, &vector->data[vector->type * pos], vector->type);
    return CDS_OK;
}

int cds_vector_front(cds_vector vector, void* out) {
    return cds_vector_at(vector, 0, out);
}

int cds_vector_back(cds_vector vector, void* out) {
    return cds_vector_at(vector, vector->size - 1, out);
}

cds_iter cds_vector_begin(cds_vector vector) {
    if (vector == NULL) {
        return NULL;
    }

    struct cds_vector_iterdata data = {.pos = 0};
    return _cds_iter_create(vector, data, false);
}

cds_iter cds_vector_rbegin(cds_vector vector) {
    if (vector == NULL) {
        return NULL;
    }

    struct cds_vector_iterdata data = {.pos = vector->size};
    return _cds_iter_create(vector, data, true);
}

cds_iter cds_vector_end(cds_vector vector) {
    if (vector == NULL) {
        return NULL;
    }

    struct cds_vector_iterdata data = {.pos = vector->size};
    return _cds_iter_create(vector, data, false);
}

cds_iter cds_vector_rend(cds_vector vector) {
    if (vector == NULL) {
        return NULL;
    }

    struct cds_vector_iterdata data = {.pos = vector->size};
    return _cds_iter_create(vector, data, true);
}

bool cds_vector_empty(cds_vector vector) {
    return vector != NULL && vector->size == 0 ? true : false;
}

size_t cds_vector_size(cds_vector vector) {
    return vector != NULL ? vector->size : 0;
}

int cds_vector_reserve(cds_vector vector, size_t capacity) {
    if (vector == NULL) {
        return CDS_ERR;
    }

    if (vector->reserved >= capacity) {
        return CDS_OK;
    }

    uint8_t* new_data = realloc(vector->data, sizeof(uint8_t) * vector->type * capacity);

    if (new_data == NULL) {
        return CDS_ERR;
    }

    vector->reserved = capacity;
    vector->data = new_data;

    return CDS_OK;
}

size_t cds_vector_capacity(cds_vector vector) {
    return vector != NULL ? vector->reserved : 0;
}

void cds_vector_shrink(cds_vector vector) {
    if (vector == NULL || vector->size >= vector->reserved) {
        return;
    }

    uint8_t* new_data = realloc(vector->data, sizeof(uint8_t) * vector->type * vector->size);

    if (new_data == NULL) {
        return;
    }

    vector->reserved = vector->size;
    vector->data = new_data;
}

void cds_vector_clear(cds_vector vector) {
    if (vector == NULL) {
        return;
    }

    for (size_t i = 0; i < vector->size; i++) {
        vector->destroyer(&vector->data[vector->type * i]);
    }

    vector->size = 0;
    vector->mod++;
}

int cds_vector_insert(cds_vector vector, size_t pos, void* data) {
    if (vector == NULL || pos >= vector->size) {
        return CDS_ERR;
    }

    if (_cds_reserve(vector) != CDS_OK) {
        return CDS_ERR;
    }

    memmove(&vector->data[vector->type * (pos + 1)], &vector->data[vector->type * pos], vector->type * (vector->size - pos));

    void* element = vector->callocator(data, vector->type);
    memcpy(&vector->data[vector->type * pos], element, vector->type);
    free(element);

    vector->size++;
    vector->mod++;

    return CDS_OK;
}

int cds_vector_erase(cds_vector vector, size_t pos) {
    if (vector == NULL || pos >= vector->size) {
        return CDS_ERR;
    }

    vector->size--;
    vector->mod++;

    vector->destroyer(&vector->data[vector->type * pos]);

    if (vector->size > pos) {
        memmove(&vector->data[vector->type * pos], &vector->data[vector->type * (pos + 1)], vector->type * (vector->size - pos));
    }

    _cds_shrink(vector);

    return CDS_OK;
}

int cds_vector_pushback(cds_vector vector, void* data) {
    if (vector == NULL) {
        return CDS_ERR;
    }

    if (_cds_reserve(vector) != CDS_OK) {
        return CDS_ERR;
    }

    void* element = vector->callocator(data, vector->type);
    memcpy(&vector->data[vector->type * vector->size], element, vector->type);
    free(element);

    vector->size++;
    vector->mod++;

    return CDS_OK;
}

int cds_vector_popback(cds_vector vector, void* out) {
    if (vector == NULL || vector->size == 0) {
        return CDS_ERR;
    }

    vector->size--;
    vector->mod++;

    memcpy(out, &vector->data[vector->type * vector->size], vector->type);
    _cds_shrink(vector);

    return CDS_OK;
}

int cds_vector_resize(cds_vector vector, size_t count, void* initializer) {
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

int cds_vector_swap(cds_vector vector, cds_vector other) {
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

static int _cds_reserve(cds_vector vector) {
    size_t size = vector->size;
    if (vector->reserved > size) {
        return CDS_OK;
    }

    int (*reserver)(cds_vector, size_t) = cds_vector_reserve;
    return size > 0 ? reserver(vector, size * 2) : reserver(vector, 8);
}

static int _cds_shrink(cds_vector vector) {
    if (vector->size == 0) {
        return CDS_OK;
    }
    if (vector->reserved / vector->size < 4) {
        return CDS_OK;
    }

    size_t new_reserved = vector->reserved / 2;
    uint8_t* new_data = realloc(vector->data, sizeof(uint8_t) * vector->type * new_reserved);

    if (new_data == NULL) {
        return CDS_ERR;
    }

    vector->reserved = new_reserved;
    vector->data = new_data;

    return CDS_OK;
}

static cds_iter _cds_iter_create(cds_vector vector, struct cds_vector_iterdata data, bool reverse) {
    struct cds_vector_iterdata* iterdata = malloc(sizeof(struct cds_vector_iterdata));

    if (iterdata == NULL) {
        return NULL;
    }

    memcpy(iterdata, &data, sizeof(struct cds_vector_iterdata));
    iterdata->mod = vector->mod;

    struct cds_iter_config config = {
        .initial_data = iterdata,
        .has_next = reverse ? _cds_iter_hasback : _cds_iter_hasnext,
        .next = reverse ? _cds_iter_back : _cds_iter_next,
        .has_back = reverse ? _cds_iter_hasnext : _cds_iter_hasback,
        .back = reverse ? _cds_iter_next : _cds_iter_back,
        .is_valid = _cds_iter_valid,
        .destroy = _cds_iter_destroy
    };

    return cds_iter_create(vector, config);
}

static bool _cds_iter_hasnext(void* structure, void** data) {
    if (structure == NULL || data == NULL) {
        return false;
    }

    cds_vector vector = structure;
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

    cds_vector vector = structure;
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

    cds_vector vector = structure;
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

    cds_vector vector = structure;
    struct cds_vector_iterdata* iterdata = *data;

    if (iterdata == NULL || vector->mod != iterdata->mod || iterdata->pos == 0) {
        return NULL;
    }

    return &vector->data[vector->type * --iterdata->pos];
}

static bool _cds_iter_valid(void* structure, void* data) {
    if (structure == NULL || data == NULL) {
        return false;
    }

    cds_vector vector = structure;
    struct cds_vector_iterdata* iterdata = data;

    return vector->mod == iterdata->mod;
}

static void _cds_iter_destroy(void* data) {
    free(data);
}

