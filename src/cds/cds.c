#include <stdlib.h>
#include <string.h>

#include <cds/cds.h>

void* cds_simple_callocator(void* src, size_t bytes) {
    void* data = malloc(bytes);
    memcpy(data, src, bytes);
    return data;
}

void cds_simple_destroyer(void* src) {
    
}

void* cds_string_callocator(void* src, size_t bytes) {
    return strdup((char*) src);
}


void cds_string_destroyer(void* src) {
    free(*(char**) src);
}

