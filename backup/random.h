#ifndef _HW3_RANDOM
#define _HW3_RANDOM

/* 
 * set_seed
 *
 * Sets the seed for our RNG. If the parameter is zero, 
 * then the seed is set to the current time, otherwise
 * it is set to the value of the parameter. 
 */
void set_seed(unsigned int seed); 

/* 
 * get_rand_range
 *
 * Returns a pseudorandom unsigned int in the range [from, to) 
 * 
 * 'from' should be less than 'to'. If not, it prints a warning to stderr and:
 *   * If from == to, it assumes the caller meant [from, from+1) and returns 'from'.
 *   * If from > to, it assumes the caller flipped them by accident and returns 
 *     a pseudorandom unsigned int in the range [to, from)
 */
unsigned int get_rand_range(unsigned int from, unsigned int to);

#endif

