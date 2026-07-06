#include <stdbool.h>

/* 
 * Operations relating to loading and saving game files. 
 */
#ifndef _HW3_FILEIO
    #define _HW3_FILEIO
    #define MAGIC_NUMBER "Dr460N5D3N"
#endif

#ifndef MAGIC_NUMBER_LENGTH
    #define MAGIC_NUMBER_LENGTH 10
#endif

int save_file(graph_t graph, player_t player, char filename[NAME_SIZE * 2]);
int load_file(char filename[NAME_SIZE * 2], graph_t **graph, player_t *player);

bool file_exists(char filname[NAME_SIZE * 2]);
int file_structure_check(char filename[NAME_SIZE * 2]);

bool magic_number_exists(int fd, int *error);