#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "graph.h"
#include "game.h"
#include "random.h"
#include "msg.h"
#include "libhw3.h"
#include "util.h"
#include "fileio.h"
#include "hw3.h"
#include "client.h"

int main (int argc, char *argv[]) 
{
	unsigned int level_factor;
	menu_action_t action;

 	if (argc > 2) {
		fprintf(stderr, "Additional arguments ignored.\n");
	}
	// Get level factor (default 0) and use it to initialize the RNG
	level_factor = 0;
	if (argc == 2) {
		level_factor = atoi(argv[1]);
	}
	set_seed(level_factor);
	
	printf(WELCOME_MSG);
	
	// there's no active game initially
	game_status_t status = GAME_OVER;

	// Initializations
	player_t player = player_init();
	char filename[NAME_SIZE * 2];
    snprintf(filename, sizeof(filename), "%s.sav", player.name);
	graph_t *gameGraph = NULL;

	// Connect to server
	server_connect("/tmp/dragons_den.sock");

	// Main loop
	bool running = true;
	do {
		action = main_menu();
		switch (action) 
		{
			case NEW:
				new(&status, &player, &gameGraph, level_factor);
				break;
			case LOAD:
				load(&status, filename, &gameGraph, &player);
				break;
			case SAVE:
				save(status, filename, gameGraph, player);
				break;
			case CONTINUE:
				continue_game(&status, gameGraph, &player);
				break;
			case HELP:
				printf(INSTRUCTIONS_MSG);
				break;
			case QUIT:
				quit(status, gameGraph, player, &running);
				break;
		}
	} while (running);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void new(game_status_t *status, player_t *player, graph_t **graph, int levelFactor)
{
	if (*status == IN_PROGRESS)
	{
		printf(CONFIRM_NEW_GAME_MSG);

		char action = 'n';
		scanf(" %c", &action);

		if (action == 'y')
		{
			free_graph(*graph);
			*graph = create_game(levelFactor, player);
			player->arrows = INIT_ARROWS;
		}
		else
		{
			return;
		}
	}
	else
	{
		free_graph(*graph);
		*graph = create_game(levelFactor, player);
		player->arrows = INIT_ARROWS;
	}

	*status = game_loop(*graph, player);
}

void quit(game_status_t status, graph_t *graph, player_t player, bool *running)
{
	if (status == IN_PROGRESS)
	{
		printf(CONFIRM_QUIT_MSG);

		char action = 'n';
		scanf(" %c", &action);

		if (action == 'n')
		{
			*running = true;
			return;
		}
		else
		{
			*running = false;
			free_graph(graph);
			return;
		}
	}

	*running = false;
	free_graph(graph);
}

void load(game_status_t *status, char filename[NAME_SIZE * 2], graph_t **graph, player_t *player)
{
	int error = 0;

	if (*status == IN_PROGRESS)
	{
		printf(CONFIRM_NEW_GAME_MSG);

		char action = 'n';
		scanf(" %c", &action);

		if (action == 'y')
		{
			if (!file_exists(filename))
			{
				printf(OPEN_ERROR_MSG);
				return;
			}

			error = file_structure_check(filename);

			if (error == -1)
			{
				printf(CORRUPTED_ERROR_MSG);
				return;
			}
			else if (error == -2)
			{
				printf(OPEN_ERROR_MSG);
				return;
			}
			else if (error == -3)
			{
				printf(READ_ERROR_MSG);
				return;
			}
		}
		else
		{
			return;
		}
	}

	if (!file_exists(filename))
	{
		printf(OPEN_ERROR_MSG);
		return;
	}

	error = load_file(filename, graph, player);

	if (error == -1)
	{
		printf(CORRUPTED_ERROR_MSG);
		return;
	}
	else if (error == -2)
	{
		printf(OPEN_ERROR_MSG);
		return;
	}
	else if (error == -3)
	{
		printf(READ_ERROR_MSG);
		return;
	}

	*status = game_loop(*graph, player);
}

void save(game_status_t status, char filename[NAME_SIZE * 2], graph_t *graph, player_t player)
{
	int error = 0;

	if (status != IN_PROGRESS)
	{
		printf(NO_GAME_IN_PROGRESS_MSG);
		return;
	}
	else
	{
		if (graph == NULL)
		{
			printf("Atempt to save file while the graph was pointing to the zero page (NULL)");
		}

		bool fileFound = file_exists(filename);

		if (fileFound)
		{
			printf(FILE_EXISTS_MSG);

			char action = 'n';
			scanf(" %c", &action);

			if (action == 'y')
			{
				error = save_file(*graph, player, filename);
			}
			else
			{
				return;
			}
		}
		else
		{
			error = save_file(*graph, player, filename);
		}		
	}

	if (error == -1)
	{
		printf(OPEN_ERROR_MSG);
	}
	else if (error == -2)
	{
		printf(WRITE_ERROR_MSG);
	}
	else
	{
		printf(SAVE_OK_MSG);
	}
}

void continue_game(game_status_t *status, graph_t *graph, player_t *player)
{
	if (*status != IN_PROGRESS)
	{
		printf(NO_GAME_IN_PROGRESS_MSG);
	}
	else
	{
		*status = game_loop(graph, player);
	}
}