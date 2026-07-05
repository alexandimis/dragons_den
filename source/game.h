#ifndef _HW3_GAME
#define _HW3_GAME

#include "graph.h"
#include <stdbool.h>

#ifndef INIT_ARROWS
#define INIT_ARROWS 2
#endif

#define NAME_SIZE 32 /* player name size */

typedef enum {
	GAME_OVER,  /* the dragon has been shot or the player has been eaten*/
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

player_t playerInit();
graph_t *createGame(int levelFactor, player_t *player);
game_status_t gameLoop(graph_t *graph, player_t *player);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void mineActivation(graph_t *graph, player_t *player);
void portalActivation(graph_t *graph, player_t *player);
void foundArrow(graph_t *graph, player_t *player);
void printIDs(vertex_t room);
bool dragonCheck(vertex_t room);
bool portalCheck(vertex_t room);