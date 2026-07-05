#include "graph.h"
#include "game.h"
#include "fileio.h"
#include "util.h"
#include "libhw3.h"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

// Save data to a file of type "player_name.sav"
//
// The function returns:
//      -1 on open fail
//      0 on normal behavior
int saveFile(graph_t graph, player_t player, char filename[NAME_SIZE * 2])
{
    // OPEN FILE
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0644);

    if (fd == -1)
    {
        return -1;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // WRITE TO FILE

    // Save the magic number
    ssize_t bytesWritten = write(fd, MAGIC_NUMBER, strlen(MAGIC_NUMBER));
    if (bytesWritten == -1) { return -2; }

    // Save player info
    bytesWritten = write(fd, &player.arrows, sizeof(player.arrows));
    if (bytesWritten == -1) { return -2; }
    bytesWritten = write(fd, &player.room->id, sizeof(player.room->id));
    if (bytesWritten == -1) { return -2; }

    // Save graph info (excluding the vertex array)
    bytesWritten = write(fd, &graph.max_id, sizeof(graph.max_id));
    if (bytesWritten == -1) { return -2; }
    bytesWritten = write(fd, &graph.num_vertices, sizeof(graph.num_vertices));
    if (bytesWritten == -1) { return -2; }

    // Save the magic number
    bytesWritten = write(fd, MAGIC_NUMBER, strlen(MAGIC_NUMBER));
    if (bytesWritten == -1) { return -2; }

    // Save vertex array
    for (int i = 0; i < graph.num_vertices; ++i)
    {
        if (graph.vertices[i] == NULL) { continue; }

        bytesWritten = write(fd, &graph.vertices[i]->id, sizeof(graph.vertices[i]->id));
        if (bytesWritten == -1) { return -2; }

        bytesWritten = write(fd, &graph.vertices[i]->contents, sizeof(contents_t));
        if (bytesWritten == -1) { return -2; }
    }

    for (int i = 0; i < graph.num_vertices; ++i)
    {
        unsigned int count = (unsigned int)getEdgeCount(*graph.vertices[i]);
        bytesWritten = write(fd, &count, sizeof(count));
        if (bytesWritten == -1) { return -2; }

        graph.vertices[i]->edges = mergeSortList(graph.vertices[i]->edges);

        edge_t *current = graph.vertices[i]->edges;

        for (int j = 0; j < count; ++j)
        {
            bytesWritten = write(fd, &current->to->id, sizeof(current->to->id));
            if (bytesWritten == -1) { return -2; }

            current = current->next;
        }
    }

    // Save the magic number
    bytesWritten = write(fd, MAGIC_NUMBER, strlen(MAGIC_NUMBER));
    if (bytesWritten == -1) { return -2; }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // CLOSE FILE
    close(fd);

    return 0;
}

// Load data from a file and sets up the game graph and player correctly
//
// The function returns:
//      -1 on wrong structure
//      -2 on open fail
//      -3 on read fail
int loadFile(char filename[NAME_SIZE * 2], graph_t **graph, player_t *player)
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1) { return -2; }

    // Check if the magic number is at the start of the file
    if (!magicNumberExists(fd, NULL)) { return -1; }

    graph_t *tempGraph = safeMalloc(sizeof(graph_t));
    tempGraph->max_id = 0;
    tempGraph->num_vertices = 0;
    tempGraph->vertices = NULL;
    player_t *tempPlayer = safeMalloc(sizeof(player_t));
    tempPlayer->room = safeMalloc(sizeof(vertex_t));

    // If arrows is < 0 then let the player have a gift of MANY arrows!
    ssize_t bytesRead = read(fd, &tempPlayer->arrows, sizeof(tempPlayer->arrows));
    if (bytesRead == -1)
    {
        safeFree((void**)&tempGraph);
        safeFree((void**)&tempPlayer->room);
        safeFree((void**)&tempPlayer);
        return -3;
    }

    int temp = 0;

    // If the id of the is < 0 then the save is corrupted
    bytesRead = read(fd, &temp, sizeof(tempPlayer->room->id));
    if (bytesRead == -1)
    {
        safeFree((void**)&tempGraph);
        safeFree((void**)&tempPlayer->room);
        safeFree((void**)&tempPlayer);
        return -3;
    }
    else if (temp < 0)
    {
        safeFree((void**)&tempGraph);
        safeFree((void**)&tempPlayer->room);
        safeFree((void**)&tempPlayer);
        return -1;
    }
    tempPlayer->room->id = (unsigned int)temp;

    // If the max_id is < 0 then the save is corrupted
    bytesRead = read(fd, &temp, sizeof(tempGraph->max_id));
    if (bytesRead == -1)
    {
        safeFree((void**)&tempGraph);
        safeFree((void**)&tempPlayer->room);
        safeFree((void**)&tempPlayer);
        return -3;
    }
    else if ((temp < 0) || (tempPlayer->room->id > temp))
    {
        safeFree((void**)&tempGraph);
        safeFree((void**)&tempPlayer->room);
        safeFree((void**)&tempPlayer);
        return -1;
    }
    tempGraph->max_id = (unsigned int)temp;

    // If num_vertices is < 0 then the save is corrupted
    bytesRead = read(fd, &temp, sizeof(tempGraph->num_vertices));
    if (bytesRead == -1)
    {
        safeFree((void**)&tempGraph);
        safeFree((void**)&tempPlayer->room);
        safeFree((void**)&tempPlayer);
        return -3;
    }
    else if ((temp < 0 || (tempGraph->max_id < temp)))
    {
        safeFree((void**)&tempGraph);
        safeFree((void**)&tempPlayer->room);
        safeFree((void**)&tempPlayer);
        return -1;
    }
    tempGraph->num_vertices = (unsigned int)temp;

    // Check for magic number check-points
    if (!magicNumberExists(fd, NULL))
    {
        safeFree((void**)&tempGraph);
        safeFree((void**)&tempPlayer->room);
        safeFree((void**)&tempPlayer);
        return -1;
    }

    tempGraph->vertices = safeMalloc(sizeof(vertex_t*) * tempGraph->num_vertices);

    for (int i = 0; i < tempGraph->num_vertices; ++i)
    {
        tempGraph->vertices[i] = NULL;
    }

    // Read the vertex array
    for (int i = 0; i < tempGraph->num_vertices; ++i)
    {
        tempGraph->vertices[i] = safeMalloc(sizeof(vertex_t));
        tempGraph->vertices[i]->edges = NULL;

        bytesRead = read(fd, &temp, sizeof(tempGraph->vertices[i]->id));
        if (bytesRead == -1)
        {
            freeGraph(tempGraph);
            safeFree((void**)&tempPlayer->room);
            safeFree((void**)&tempPlayer);
            return -3;
        }
        else if ((temp < 0) || (temp > tempGraph->max_id))
        {
            freeGraph(tempGraph);
            safeFree((void**)&tempPlayer->room);
            safeFree((void**)&tempPlayer);
            return -1;
        }
        tempGraph->vertices[i]->id = (unsigned int)temp;

        contents_t tempContents = EMPTY;

        // If contents is not an expected value then the save is corrupted
        bytesRead = read(fd, &tempContents, sizeof(tempGraph->vertices[i]->contents));
        if (bytesRead == -1)
        {
            freeGraph(tempGraph);
            safeFree((void**)&tempPlayer->room);
            safeFree((void**)&tempPlayer);
            return -3;
        }
        else if (
        tempContents != EMPTY &&
        tempContents != PORTAL &&
        tempContents != MINE &&
        tempContents != ARROW &&
        tempContents != DRAGON
        )
        {
            freeGraph(tempGraph);
            safeFree((void**)&tempPlayer->room);
            safeFree((void**)&tempPlayer);
            return -1;
        }
        tempGraph->vertices[i]->contents = tempContents;
    }

    // Initialize the temp edges
    for (int i = 0; i < tempGraph->num_vertices; ++i)
    {
        if (tempGraph->vertices[i] != NULL)
        {
            tempGraph->vertices[i]->edges = NULL;
        }
    }

    // Read each vertex's edge list
    for (int i = 0; i < tempGraph->num_vertices; ++i)
    {
        unsigned int neighbors = 0;

        // If neighbors is < 0 then the save is corrupted
        bytesRead = read(fd, &temp, sizeof(neighbors));
        if (bytesRead == -1)
        {
            freeGraph(tempGraph);
            safeFree((void**)&tempPlayer->room);
            safeFree((void**)&tempPlayer);
            return -3;
        }
        else if ((temp < 0) || (temp > tempGraph->num_vertices))
        {
            freeGraph(tempGraph);
            safeFree((void**)&tempPlayer->room);
            safeFree((void**)&tempPlayer);
            return -1;
        }

        neighbors = (unsigned int)temp;

        edge_t *current = tempGraph->vertices[i]->edges;

        for (int j = 0; j < neighbors; ++j)
        {
            unsigned int id = 0;
            edge_t *edge = safeMalloc(sizeof(edge_t));

            // If id is < 0 then the save is corrupted
            bytesRead = read(fd, &temp, sizeof(id));
            if (bytesRead == -1)
            {
                freeGraph(tempGraph);
                safeFree((void**)&tempPlayer->room);
                safeFree((void**)&tempPlayer);
                return -3;
            }
            else if ((temp < 0) || (temp > tempGraph->max_id))
            {
                freeGraph(tempGraph);
                safeFree((void**)&tempPlayer->room);
                safeFree((void**)&tempPlayer);
                return -1;
            }
            id = (unsigned int)temp;

            edge->to = getRoom(tempGraph, id);
            edge->next = NULL;

            if (current == NULL)
            {
                current = edge;
                tempGraph->vertices[i]->edges = current;
            }
            else
            {
                current->next = edge;
                current = current->next;
            }
        }
    }

    // Check if the magic number is at the end of the file
    if (!magicNumberExists(fd, NULL))
    {
        freeGraph(tempGraph);
        safeFree((void**)&tempPlayer->room);
        safeFree((void**)&tempPlayer);
        return -1;
    }

    // Assign the graph to the passed one after freeing
    freeGraph(*graph);
    *graph = tempGraph;

    // Assign player info to the passed player
    player->arrows = tempPlayer->arrows;
    strcpy(player->name, tempPlayer->name);
    player->room = getRoom(*graph, tempPlayer->room->id);

    // Free the temp player
    safeFree((void**)&tempPlayer->room);
    safeFree((void**)&tempPlayer);    

    return 0;
}

// Checks if the file with the given name exists in the save file directory
bool fileExists(char filename[NAME_SIZE * 2])
{
    if (access(filename, F_OK) != 0)
    {
        return false;
    }
    
    

    return true;
}

// Checks if the save file has the correct structure for reading
//
// The function returns:
//      -1 on open fail
//      -2 on read fail
//      -3 on incorrect structure
int fileStructureCheck(char filename[NAME_SIZE * 2])
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1) { return -1; }

    int error = 0;

    if (!magicNumberExists(fd, &error))
    {
        if (error == -1) { return -2; }
        return -3;
    }

    off_t bytesMoved = lseek(fd, 3 * sizeof(unsigned int), SEEK_CUR);
    if (bytesMoved == -1) { return -2; } // There wasn't another error option for lseek so...

    unsigned int vertexCount = 0;
    int temp = 0;
    ssize_t bytesRead = read(fd, &temp, sizeof(vertexCount));

    if (bytesRead == -1) { return -2; }
    else if (temp < 0) { return -3; }
    vertexCount = (unsigned int)temp;

    if (!magicNumberExists(fd, &error))
    {
        if (error == -1) { return -2; }
        return -3;
    }

    bytesMoved = lseek(fd, vertexCount * (sizeof(unsigned int) + sizeof(contents_t)), SEEK_CUR);
    if (bytesMoved == -1) { return -2; }

    for (int i = 0; i < vertexCount; ++i)
    {
        unsigned int neighbors = 0;
        bytesRead = read(fd, &temp, sizeof(neighbors));

        if (bytesRead == -1) { return -2; }
        else if (temp < 0) { return -3; }

        bytesMoved = lseek(fd, temp * sizeof(unsigned int), SEEK_CUR);
        if (bytesMoved == -1) { return -2; }
    }

    if (!magicNumberExists(fd, &error))
    {
        if (error == -1) { return -2; }
        return -3;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// Checks if the magic number is at the given place
bool magicNumberExists(int fd, int *error)
{
    char magic[MAGIC_NUMBER_LENGTH];
    ssize_t bytesRead = read(fd, magic, MAGIC_NUMBER_LENGTH);

    if (bytesRead == -1)
    {
        if (error != NULL)
        {
            *error = -1;
        }

        return false;
    }

    for (int i = 0; i < MAGIC_NUMBER_LENGTH; ++i)
    {
        if (magic[i] != MAGIC_NUMBER[i])
        {
            return false;
        }
    }

    return true;
}