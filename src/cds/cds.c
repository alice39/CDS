#include <stdlib.h>

#include <cds/cds.h>

struct cds_memory cds_memory_system() {
    return (struct cds_memory) {
        .allocator = malloc,
        .reallocator = realloc,
        .deallocator = free
    };
}

bool cds_memory_valid(struct cds_memory memory) {
    if (memory.allocator == NULL) {
        return false;
    }
    if (memory.reallocator == NULL) {
        return false;
    }
    if (memory.deallocator == NULL) {
        return false;
    }

    return true;
}

