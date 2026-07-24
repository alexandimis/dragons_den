#include <stdio.h>
#include <stdlib.h>

#include "util.h"

// If the passed pointer is NULL, the program exits with code EXIT_FAILURE
void memoryErrorCheck(void *ptr)
{
    if (ptr == NULL)
    {
        printf("Memory error.\n");
        exit(EXIT_FAILURE);
    }
}

// If malloc fails the program exits
void *safe_malloc(size_t size)
{
    if (size == 0) { return NULL; }

    void *ptr = malloc(size);
    memoryErrorCheck(ptr);
    return ptr;
}

// If realloc fails the program exits
void *safe_realloc(void *ptr, size_t size)
{
    ptr = realloc(ptr, size);

    if (size == 0) { return ptr; }

    memoryErrorCheck(ptr);
    return ptr;
}

// Sets pointer to NULL after free
void safe_free(void **ptr)
{
    free(*ptr);
    *ptr = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////