#ifndef _HW3
    #define _HW3
#endif

void new(game_status_t *status, player_t *player, graph_t **graph, int levelFactor);
void load(game_status_t *status, char filename[NAME_SIZE * 2], graph_t **graph, player_t *player);
void save(game_status_t status, char filename[NAME_SIZE * 2], graph_t *graph, player_t player);
void continueGame(game_status_t *status, graph_t *graph, player_t *player);
void help();
void quit(game_status_t status, graph_t *graph, player_t player);