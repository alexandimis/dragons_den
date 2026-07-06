#ifndef _HW3_LIB
#define _HW3_LIB
#include "graph.h"

/* 
 * create_game_graph
 *
 * Creates a dynamically allocated game graph, populates and returns it. 
 * 
 * Its size depends of the value of (level_factor % 3) as follows:
 * If == 1 -> an even number of vertices between 8 and 15
 * If == 2 -> an even number of vertices between 16 and 23
 * If == 0 -> an even number of vertices between 24 and 31
 *
 * The graph is guaranteed to be connected. 
 *
 * Note: The function allocates memory dynamically for :
 * - the object of type graph_t * that it returns
 * - the dynamic table 'vertices'
 * - the object of type vertex_t * that is stored in vertices[i]
 * - the list of edges attached to each vertex_t* object
 */
graph_t *create_game_graph(int level_factor);

/* 
 * print_graph
 *
 * Prints the contents of the given graph.
 * The vertices appear in the same order as in the 'vertices' array.
 * The neighbors of each vertex appear sorted by id.
 */
void print_graph(graph_t*);

#endif
