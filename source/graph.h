#pragma once
#include <stdbool.h>

#ifndef _HW3_GRAPH
#define _HW3_GRAPH

typedef enum {
	EMPTY, PORTAL, MINE, ARROW, DRAGON
} contents_t;

struct _edge;

typedef struct  _vertex {
	unsigned int id; // unique id
	contents_t contents;
	struct _edge *edges; // a vertex connects to a list of edges
} vertex_t;

typedef struct _edge {
	vertex_t *to;  // the other adjacent vertex
	struct _edge *next;
} edge_t;
	
typedef struct {
	unsigned int max_id; // maximum vertex id given at any time
	unsigned int num_vertices;
	vertex_t **vertices;  // the graph is a dynamic array of pointers to vertices
} graph_t;


#endif

/* Graph Functions */

graph_t *empty_graph_init();
void add_vertex(graph_t *graph, contents_t contents);
void link_vertices(vertex_t *v1, vertex_t *v2);
void unlink_vertices(vertex_t *v1, vertex_t *v2);
void remove_vertex(graph_t *graph, vertex_t *vertex);
void free_graph(graph_t *graph);

vertex_t *create_vertex(contents_t contents, unsigned int id);
vertex_t *get_room(graph_t *graph, unsigned int id);
int get_edge_count(vertex_t room);
bool neighbour_found(vertex_t room, unsigned int id);

// Sorting functions
edge_t *sort_list(edge_t *head);
edge_t *split_list(edge_t *head);
edge_t *merge_list(edge_t *first, edge_t *second);