#include <stdio.h>
#include <time.h>
#include <limits.h>
#include "random.h"

static unsigned int _seed;

/* Sets the seed */
void set_seed(unsigned int seed) 
{
	if (seed == 0) {
		_seed = (unsigned int)time(NULL);
	}
	else {
		_seed = seed;
	}
}

/* Returns a random unsigned int */
static unsigned int get_rand_int() 
{
	_seed ^= _seed << 13;
	_seed ^= _seed << 17;
	_seed ^= _seed << 5;
	return _seed;
}

/* Returns a random unsigned int in the range [from, to).
 * Also handles the cases when from >= to */
unsigned int get_rand_range(unsigned int from, unsigned int to) 
{
	static unsigned int prev = UINT_MAX;
	if (to == from) {
		fprintf(stderr, "WARNING: range of zero size in get_rand_range.\n");
		return from;
	}
	if (to < from) {  /* swap them */
		to = to ^ from;
		from = to ^ from;
		to = to ^ from;
		fprintf(stderr, "WARNING: range of negative direction in get_rand_range.\n");
	}
	unsigned int next = get_rand_int() % (to-from) + from;
	
	// avoid consecutive same numbers if possible
	if (next == prev && to-from >= 2) {
		if (next + 1 < to) { // next is guaranteed < UINT_MAX so will not overflow
			next++;
		}
		else if (next-1 >= from) { // here, next is guaranteed > 0 so will not underflow
			next--;
		}
	}
	prev = next;
	return next;
}

