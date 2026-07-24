#include "msg.h"
#include "util.h"
#include "libhw3.h"
#include "random.h"
#include "graph.h"
#include "game.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>

/*
 * main_menu
 * 
 * Prints the main game menu, reads and returns the user's requested action.
 */
menu_action_t main_menu() 
{
	char action;
	while (1) {
		printf(MAIN_MENU_MSG);
		scanf(" %c", &action);
		switch (toupper(action)) {
			case 'N':
				return NEW;
			case 'L':
				return LOAD;
			case 'S':
				return SAVE;
			case 'C':
				return CONTINUE;
			case 'Q':
				return QUIT;
			case 'H':
				return HELP;
			default:
				while (getchar() != '\n'); 
				printf(WRONG_SELECTION_MSG);
		}	
	}
	return QUIT; // unreachable
}

/*
 * in_game_menu
 * 
 * Prints the menu of options available while a game is in progress,
 * reads and returns the user's requested action.
 */
game_action_t in_game_menu() 
{
	char action;
	while(1) {
		printf(GAME_MENU_MSG);
		scanf(" %c", &action);
		switch (toupper(action)) {
			case 'M': 
				return MOVE;
			case 'S': 
				return SHOOT;
			case 'E': 
				return EXIT;
			default: 
				while (getchar() != '\n');
				printf(WRONG_SELECTION_MSG);
		}
	}
}

// Initialize the player struct
player_t player_init()
{
	// Name initialization
	printf(NAME_REQUEST_MSG);
	char name[NAME_SIZE + 1];
	fgets(name, NAME_SIZE, stdin);
	name[strcspn(name, "\n")] = '\0'; // Replaces the \n from ENTER (if found) with a '\0' 
	printf(GREETING_MSG, name);

	// Player struct init
	player_t player;
	player.arrows = INIT_ARROWS;
	strcpy(player.name, name);
	player.room = NULL;

	return player;
}

// Create the game graph and set the player's location
graph_t *create_game(int levelFactor, player_t *player)
{
	graph_t *graph =  create_game_graph(levelFactor);
	player->room = graph->vertices[0];

	return graph;
}

// The game loop
game_status_t game_loop(graph_t *graph, player_t *player)
{
	while (1)
	{
		#ifdef DEBUG
			print_graph(graph);
		#endif

		printf(ENTER_ROOM_MSG);
		contents_t contents = player->room->contents;

		switch (contents)
		{
			case MINE:
				printf(TRIGGERED_MINE_MSG);
				mine_activation(graph, player);
				continue;
			case PORTAL:
				printf(TRIGGERED_PORTAL_MSG);
				portal_activation(graph, player);
				continue;
			case DRAGON:
				printf(ENCOUNTERED_DRAGON_MSG);
				return GAME_OVER;
			case ARROW:
				found_arrow(graph, player);
				printf(FOUND_ARROW_MSG, player->arrows);
				break;
			case EMPTY:
				break;
			default: // Non-reachable (on normal behavior)
				break;
		}

		// Sort the edge_t list for the player room
		player->room->edges = sort_list(player->room->edges);

		printf(PASSAGE_INFO_MSG, get_edge_count(*player->room));
		print_ids(*player->room);

		// Check if the dragon is nearby
		if (dragon_check(*player->room))
		{
			printf(DETECTED_DRAGON_MSG);
		}

		// Check if a portal is nearby
		if (portal_check(*player->room))
		{
			printf(DETECTED_PORTAL_MSG);
		}

		while (1)
		{
			game_action_t action = in_game_menu();

			if (action == EXIT)
			{
				return IN_PROGRESS;
			}
			else if (action == MOVE)
			{
				unsigned int id = 0;
				scanf(" %d", &id);

				if (!neighbour_found(*player->room, id))
				{
					printf(NO_PASSAGE_MSG);
					continue;
				}

				player->room = get_room(graph, id);
				break;
			}
			else if (action == SHOOT)
			{
				unsigned int id = 0;
				scanf(" %d", &id);
			
				if (!neighbour_found(*player->room, id))
				{
					printf(NO_PASSAGE_MSG);
					continue;
				}
				else if (player->arrows <= 0)
				{
					printf(NO_ARROWS_MSG);
					continue;
				}
				else
				{
					--(player->arrows);

					vertex_t *temp = get_room(graph, id);

					if (temp->contents == DRAGON)
					{
						printf(SHOT_DRAGON_MSG);
						return GAME_OVER;
					}
					else
					{
						printf(ARROW_MISSED_MSG, player->arrows);
					}
				}

				break;
			}
		}
	}
}

game_status_t multiplayer_game_loop(player_t *player)
{
	// Sync player data with server
	game_status_t status = IN_PROGRESS;

	while (1)
	{
		in_game_menu();
	}

	return status;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// When a mine is activated the room vertex gets deleted
void mine_activation(graph_t *graph, player_t *player)
{
	if ((graph == NULL) || (player == NULL)) { return; }

	// Edge case handling
	if (graph->num_vertices <= 1)
	{
		printf("ONLY ONE ROOM LEFT, CANNOT EXPLODE IT\n");
		return;
	}

	remove_vertex(graph, player->room);
	player->room = graph->vertices[get_rand_range(0, graph->num_vertices)];
}

// When a portal is activated a room vertex gets added
// and the player is teleported to a random room
void portal_activation(graph_t *graph, player_t *player)
{
	if ((graph == NULL) || (player == NULL)) { return; }

	// The new vertex will be at pos graph.num_vertices - 1
	add_vertex(graph, EMPTY);

	// The position in the vertex array of the new room
	unsigned int new = graph->num_vertices - 1;

	unsigned int r1 = get_rand_range(0, new);
	unsigned int r2 = 0;

	while (1)
	{
		r2 = get_rand_range(0, new);
		if (r1 != r2) { break; }
	}

	// Links the new room with 2 random rooms
	link_vertices(graph->vertices[new], graph->vertices[r1]);
	link_vertices(graph->vertices[new], graph->vertices[r2]);

	// Sets player position to the new room
	player->room = graph->vertices[new];
}

// Player gets +1 arrow from the room, and the room becomes EMPTY
void found_arrow(graph_t *graph, player_t *player)
{
	++(player->arrows);
	player->room->contents = EMPTY;
}

// Prints the room's neighbor's IDs in increasing order
void print_ids(vertex_t room)
{
	edge_t *current = room.edges;

	while (current != NULL)
	{
		printf(" %d", current->to->id);
		current = current->next;
	}

	printf("\n");
}

// Returns true if the dragon is lurking in a room connected with the given one
bool dragon_check(vertex_t room)
{
	edge_t *current = room.edges;

	while (current != NULL)
	{
		if (current->to->contents == DRAGON)
		{
			return true;
		}

		current = current->next;
	}

	return false;
}

// Returs true if there is a portal in a room connected wutg the given one
bool portal_check(vertex_t room)
{
	edge_t *current = room.edges;

	while (current != NULL)
	{
		if (current->to->contents == PORTAL)
		{
			return true;
		}

		current = current->next;
	}

	return false;
}