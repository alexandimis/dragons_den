#ifndef _HW3_GAME
#define _HW3_GAME

#include "graph.h"
#include <stdbool.h>

#ifndef INIT_ARROWS
#define INIT_ARROWS 2
#endif

#define NAME_SIZE 32 /* player name size */

typedef enum {
	GAME_OVER,  /* the dragon has been shot, the player has been eaten or the player exit the game*/
	IN_PROGRESS /* the game is still ongoing */
} game_status_t;

typedef enum {
    QUIT, NEW, LOAD, SAVE, CONTINUE, HELP
} menu_action_t;

typedef enum {
    EXIT, /* stop/interrupt the current game */
	MOVE, /* move to a neighboring room */
	SHOOT /* shoot an arrow towards a neighboring room */
} game_action_t;
                                                                                
typedef struct {
    char name[NAME_SIZE];
	unsigned int arrows;
	vertex_t *room;  /* player's location */
} player_t;


/*
 * main_menu
 *
 * Prints the main game menu and returns the user's requested action.
 */
menu_action_t main_menu();

/*
 * in_game_menu
 * 
 * Prints the menu of options available while a game is in progress
 * and returns the user's requested action.
 */
game_action_t in_game_menu();
	
#endif

player_t player_init();
graph_t *create_game(int levelFactor, player_t *player);
game_status_t game_loop(graph_t *graph, player_t *player);
game_status_t multiplayer_game_loop(player_t *player);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void mine_activation(graph_t *graph, player_t *player);
void portal_activation(graph_t *graph, player_t *player);
void found_arrow(graph_t *graph, player_t *player);
void print_ids(vertex_t room);
bool dragon_check(vertex_t room);
bool portal_check(vertex_t room);