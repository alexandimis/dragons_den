#include "graph.h"

#include <stddef.h>

#pragma once

/* HELFPFUL FUNCTIONS */

void memoryErrorCheck(void *ptr);

void *safe_malloc(size_t size);
void *safe_realloc(void *ptr, size_t size);
void safe_free(void **ptr);