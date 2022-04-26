#ifndef CDS_GUARD_HEADER
#define CDS_GUARD_HEADER

#include <stddef.h>
#include <stdbool.h>

#define CDS_OBJ(type) void*

#define CDS_OK 0
#define CDS_ERR 1

typedef void* (*cds_allocator)(size_t bytes);
typedef void* (*cds_reallocator)(void* ptr, size_t bytes);
typedef void (*cds_deallocator)(CDS_OBJ(T) src);

struct cds_memory {
    // allocator for internal
    cds_allocator allocator;
    // realocator for internal
    cds_reallocator reallocator;
    // deallocator for internal
    cds_deallocator deallocator;
};

struct cds_memory cds_memory_system();
bool cds_memory_valid(struct cds_memory memory);

#endif // CDS_GUARD_HEADER
