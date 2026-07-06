#include "graph.h"
#include "util.h"

#include <stdio.h>

// Creates an empty graph
graph_t *empty_graph_init()
{
    graph_t *graph = safe_malloc(sizeof(graph_t));

    graph->max_id = 0;
    graph->num_vertices = 0;
    graph->vertices = NULL;

    return graph;
}

// Adds a new vertex in the vertex array (newly allocated)
void add_vertex(graph_t *graph, contents_t contents)
{
    if (graph == NULL) { return; }

    // Increase vertex array size by 1
    ++(graph->num_vertices);
    graph->vertices = safe_realloc(graph->vertices, sizeof(vertex_t*) * (graph->num_vertices));
    
    // Create new EMPTY vertex and adds it at the end of the array
    vertex_t *vertex = create_vertex(contents, ++(graph->max_id));
    graph->vertices[graph->num_vertices - 1] = vertex;
}

// Links two vertices
void link_vertices(vertex_t *v1, vertex_t *v2)
{
    if ((v1 == NULL) || (v2 == NULL)) { return; }

    // Add v2 to v1's edge list
    if (v1->edges == NULL)
    {
        v1->edges = safe_malloc(sizeof(edge_t));
        v1->edges->to = v2;
        v1->edges->next = NULL;
    }
    else
    {
        edge_t *new = safe_malloc(sizeof(edge_t));

        new->to = v2;
        new->next = v1->edges;
        v1->edges = new;
    }

    // Add v1 to v2's edge list
    if (v2->edges == NULL)
    {
        v2->edges = safe_malloc(sizeof(edge_t));
        v2->edges->to = v1;
        v2->edges->next = NULL;
    }
    else
    {
        edge_t *new = safe_malloc(sizeof(edge_t));

        new->to = v1;
        new->next = v2->edges;
        v2->edges = new;
    }
}

// Unlinks two vertices
void unlink_vertices(vertex_t *v1, vertex_t *v2)
{
    if ((v1 == NULL) || (v2 == NULL)) { return; }

    edge_t *current = NULL;

    // Remove v2 from v1's edge list
    current = v1->edges;

    if (current == NULL) // If list is empty, SKIP
    {
        /* Do Nothing */
    }
    else if (current->next == NULL)
    {
        v1->edges = NULL;
        safe_free((void**)&current);
    }
    else
    {
        edge_t *previous = NULL;

        while (current != NULL)
        {
            if (current->to == v2)
            {
                if (previous == NULL) // First entry removal
                {
                    v1->edges = current->next;
                    safe_free((void**)&current);
                    break;
                }
                else if (current->next == NULL) // Last entry removal
                {
                    previous->next = NULL;
                    safe_free((void**)&current);
                    break;
                }
                else // In between
                {
                    previous->next = current->next;
                    safe_free((void**)&current);
                    break;
                }
            }

            previous = current;
            current = current->next;
        }
    }    

    // Remove v1 from v2's edge list
    current = v2->edges;

    if (current == NULL) // If list is empty, SKIP
    {
        /* Do Nothing */
    }
    else if (current->next == NULL)
    {
        v2->edges = NULL;
        safe_free((void**)&current);
    }
    else
    {
        edge_t *previous = NULL;

        while (current != NULL)
        {
            if (current->to == v1)
            {
                if (previous == NULL) // First entry removal
                {
                    v2->edges = current->next;
                    safe_free((void**)&current);
                    break;
                }
                else if (current->next == NULL) // Last entry removal
                {
                    previous->next = NULL;
                    safe_free((void**)&current);
                    break;
                }
                else // In between
                {
                    previous->next = current->next;
                    safe_free((void**)&current);
                    break;
                }
            }

            previous = current;
            current = current->next;
        }
    }    
}

// Removes a single vertex from the vertices array
void remove_vertex(graph_t *graph, vertex_t *vertex)
{
    for (int i = 0; i < graph->num_vertices; ++i)
    {
        if (graph->vertices[i] == vertex)
        {
            edge_t *current = vertex->edges;

            while (current != NULL)
            {
                edge_t *previous = current;
                current = current->next;
                unlink_vertices(vertex, previous->to);
            }

            vertex_t *temp = graph->vertices[i];
            graph->vertices[i] = graph->vertices[graph->num_vertices - 1];
            graph->vertices[graph->num_vertices - 1] = temp;

            // Free the vertex for removal
            safe_free((void**)&graph->vertices[graph->num_vertices - 1]);
            --(graph->num_vertices);
        }
    }
}

// Frees the graph
void free_graph(graph_t *graph)
{
    if (graph == NULL) { return; }

    if (graph->vertices != NULL)
    {
        for (int i = 0; i < graph->num_vertices; ++i)
        {
            if (graph->vertices[i] == NULL) { continue; }
            edge_t *current = graph->vertices[i]->edges;

            while (current != NULL)
            {
                edge_t *next = current->next;

                safe_free((void**)&current);
                current = next;
            }

            safe_free((void**)&graph->vertices[i]);
        }
    }
    safe_free((void**)&graph->vertices);

    safe_free((void**)&graph);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// Creates a vertex with the given parameters
vertex_t *create_vertex(contents_t contents, unsigned int id)
{
    vertex_t *vertex = safe_malloc(sizeof(vertex_t));

    vertex->id = id;
    vertex->contents = contents;
    vertex->edges = NULL;

    return vertex;
}

// Returns a room based on a given id
vertex_t *get_room(graph_t *graph, unsigned int id)
{
    vertex_t *room = NULL;

    for (int i = 0; i < graph->num_vertices; ++i)
    {
        if (id == graph->vertices[i]->id)
        {
            room = graph->vertices[i];
            break;
        }
    }

    return room;
}

// Returns the number of edges/passages connected with the given room
int get_edge_count(vertex_t room)
{
    edge_t *current = room.edges;
    int count = 0;

    while (current != NULL)
    {
        ++count;
        current = current->next;
    }

    return count;
}

// Checks if the give id is a neighbour of the given room
bool neighbour_found(vertex_t room, unsigned int id)
{
    edge_t *current = room.edges;

    while (current != NULL)
    {
        if (current->to->id == id)
        {
            return true;
        }

        current = current->next;
    }

    return false;
}

// Sorting functions

// Merge sort for the linked list edge_t
edge_t *sort_list(edge_t *head)
{
    if ((head == NULL) || (head->next == NULL)) { return head; }

    edge_t *second = split_list(head);

    head = sort_list(head);
    second = sort_list(second);

    return merge_list(head, second);
}

// Splits the linked list in two
edge_t *split_list(edge_t *head)
{
    edge_t *fast = head;
    edge_t *slow = head;

    while ((fast != NULL) && (fast->next != NULL))
    {
        fast = fast->next->next;

        if (fast != NULL)
        {
            slow = slow->next;
        }
    }

    edge_t *temp = slow->next;
    slow->next = NULL;

    return temp;
}

// Merges two linked lists
edge_t *merge_list(edge_t *first, edge_t *second)
{
    if (first == NULL) { return second; }
    if (second == NULL) { return first; }

    if (first->to->id < second->to->id)
    {
        first->next = merge_list(first->next, second);
        return first;
    }
    else
    {
        second->next = merge_list(first, second->next);
        return second;
    }
}