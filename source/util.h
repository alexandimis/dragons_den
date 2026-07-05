#include "graph.h"

#include <stddef.h>

#pragma once

/* HELFPFUL FUNCTIONS */

void memoryErrorCheck(void *ptr);

void *safeMalloc(size_t size);
void *safeRealloc(void *ptr, size_t size);
void safeFree(void **ptr);