#ifndef CDS_GUARD_HEADER
#define CDS_GUARD_HEADER

#include <stddef.h>

#define CDS_OBJ(type) void*

#define CDS_OK 0
#define CDS_ERR 1

// copy allocator
typedef void* (*cds_callocator)(void* src, size_t bytes);
typedef void (*cds_destroyer)(void* src);

// primitive allocator
void* cds_simple_callocator(void* src, size_t bytes);
// primitive destroyer
void cds_simple_destroyer(void* src);

void* cds_string_callocator(void* src, size_t bytes);
void cds_string_destroyer(void* src);

#endif // CDS_GUARD_HEADER
