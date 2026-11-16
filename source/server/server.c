#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>

#include "../game/game.h"
#include "../game/libhw3.h"
#include "../game/msg.h"
#include "../game/graph.h"
#include "server.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// Global variables
int server_fd;
struct sockaddr_un addr;

pthread_mutex_t mutex_lock;
pthread_mutex_t user_array_lock;

graph_t *game_map;

user_t users[MAX_PLAYERS];
room_attack_t room_attack[MAX_PLAYERS];
int connected_users;

bool update_needed = false;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// THREAD FUNCTIONS

// Whenever a user connects to the server, add them to the user list
void *user_connect(void *)
{
    socklen_t addr_len = sizeof(addr);

    while (1) {
        // If lobby is full, wait 1s
        if (connected_users >= MAX_PLAYERS) {
            sleep(1);
            continue;
        }
        
        int client_fd = accept(server_fd, (struct sockaddr *)&addr, &addr_len);
        if (client_fd == -1) { 
            perror("accept() failed: ");
            return NULL;
        }

        // Set the client socket to NON_BLOCKING (server doesn't wait for data that is not there)
        if(fcntl(client_fd, F_SETFL, fcntl(client_fd, F_GETFL) | O_NONBLOCK) < 0) {
            perror("fcntl() failed: ");
            continue;
        }

        pthread_mutex_lock(&user_array_lock);
            char player_name[NAME_SIZE];
            int bytes_read = recv(client_fd, player_name, sizeof(char) * NAME_SIZE, 0);
            if (bytes_read == -1) {
                perror("recv() failed: ");
                close(client_fd);
                return NULL;
            }
            
            user_t new;
            strcpy(new.player.name, player_name);

            new.online = true;
            new.id = -1;
            new.socket_fd = client_fd;
            new.action = NOTHING;

            if (user_exists_database()) { // Currently this will never go
                // Handle that
            } else {
                new.player.arrows = INIT_ARROWS;
                new.player.room = game_map->vertices[0]; // Spawn new players in the first room
            }

            send(client_fd, &new.player.arrows, sizeof(new.player.arrows), 0);
            send(client_fd, &new.player.room->id, sizeof(new.player.room->id), 0);
            send(client_fd, &new.player.room->contents, sizeof(new.player.room->contents), 0);
            
            add_user(new);

            // NOTE: access the variable ONLY within mutex_lock(&user_array_lock)
            update_needed = true;
        pthread_mutex_unlock(&user_array_lock);

        sleep(1);
    }

    return NULL;
}

// Every second it scans the user list for offline users,
// if a user is offline he gets removed from the user list
void *user_disconnect(void *)
{
    while (1) {
        pthread_mutex_lock(&user_array_lock);
            for (int i = 0; i < MAX_PLAYERS; ++i) {
                if ((users[i].online) || (users[i].id < 0)) {
                    continue;
                } else if (remove_user(users[i]) == -1) {
                    perror("remove_user() failed: ");
                } else {
                    update_needed = true;               // I don't fucking know why it's here
                }
            }
        pthread_mutex_unlock(&user_array_lock);
        
        sleep(1);
    }

    return NULL;
}

// Constantly listens for pings from users. If a user doesnt ping in a set time they are considered
// offline
void *ping_listen(void *)
{
    // while (1) {
    //     // Listen for each users reply? If yes what should the timeout be...
    //     sleep(5);
    //     print_users();
    // }

    return NULL;
}

void *check_user_input(void *)
{
    while (1) {
        // Poll for input
        pthread_mutex_lock(&mutex_lock);
        
        for (int i = 0; i < MAX_PLAYERS; ++i)
        {
            game_action_t action;
            
            if (recv(users[i].socket_fd, &action, sizeof(action), 0) > 0) // Player wants to do an action
            {
                print_action(action);
                update_needed = true;

                if (action == EXIT)
                {
                    users[i].online = false;
                    users[i].action = EXIT;
                }
                else if (action == MOVE)
                {
                    unsigned int id = 0;

                    if (recv(users[i].socket_fd, &id, sizeof(id), 0) > 0) // Player specified where to move
                    {
                        users[i].action = MOVE;
                        users[i].action_towards = id;
                        printf("TO %u\n", id);
                    } else
                    {
                        printf(UNSPECIFIED_ROOM_ID);
                        update_needed = false;
                    }
                } else if (action == SHOOT)
                {
                    unsigned int id = 0;

                    if (recv(users[i].socket_fd, &id, sizeof(id), 0) > 0) // Player specified where to shoot
                    {
                        users[i].action = SHOOT;
                        users[i].action_towards = id;
                        printf("TO %u\n", id);
                    } else
                    {
                        printf(UNSPECIFIED_ROOM_ID);
                        update_needed = false;
                    }
                }   
            }
        }
        pthread_mutex_unlock(&mutex_lock);
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
    // User array and attack init
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        users[i].id = -2;
        users[i].online = false;
        users[i].socket_fd = -2;
        room_attack[i].attack = EMPTY;
        room_attack[i].id = 0;
    }

    // Game graph init
    game_map = NULL;
    while (1)
    {
        printf("Create new map (N) or load an existing one (L): ");
        char map_creation = 'a';
        scanf(" %c", &map_creation);
        if ((map_creation == 'N') || (map_creation == 'n')) { game_map = create_game_graph(LEVEL_FACTOR); break; } 
        else if ((map_creation == 'L') || (map_creation == 'l')) { /* LOAD GRAPH FROM FILE */ break; }
        else { printf(WRONG_SELECTION_MSG); }
    }
    print_graph(game_map);

    // Socket fd initialization
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) { return -1; }
    char *system_path = "/tmp/dragons_den.sock";
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, system_path);
    unlink(system_path); // If the socket already exists... start fresh (remove it)
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("bind() failed: ");
        return -1;
    }
    listen(server_fd, MAX_PLAYERS);

    // Thread creations
    connected_users = 0;
    pthread_t connect_thread; // User connection
    pthread_create(&connect_thread, NULL, user_connect, NULL);
    pthread_t disconnect_thread; // Offline user removal
    pthread_create(&disconnect_thread, NULL, user_disconnect, 0);
    pthread_t pinger; // User ping
    pthread_create(&pinger, NULL, ping_listen, NULL);
    pthread_t user_input; // User input
    pthread_create(&user_input, NULL, check_user_input, NULL);

    bool running = true;
    while (running)
    {
        // Update game state
        if (update_needed)
        {
            update_game_state();
            
            // Send players new game state

            #ifdef PRINT_MAP
                pthread_mutex_lock(&mutex_lock);
                print_graph(game_map);
                pthread_mutex_unlock(&mutex_lock);
            #endif

            #ifdef PRINT_USERS
                print_users();
            #endif
        }
    }

    // Wait for the threads to finish
    pthread_join(connect_thread, NULL);
    pthread_join(disconnect_thread, NULL);
    pthread_join(pinger, NULL);
    pthread_join(user_input, NULL);
    close(server_fd);
    unlink(system_path);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// Adds a user in the array's first available spot
// If no space is available it simply returns
void add_user(user_t user)
{
    bool found = true;
    int position = get_user_pos(user, &found);
    if (position == -1) // Error
    {
        perror("get_user_pos() failed: ");
        return;
    } else if (found == true) // User FOUND.. don't ask why
    {
        perror("add_user() tried to add an existing user: ");
        return;
    }

    user.id = position;
    users[position] = user;
    ++connected_users;
}

// Removes a user from the array
// On error it simply returns -1
int remove_user(user_t user)
{
    bool found = false;
    int position = get_user_pos(user, &found);
    if (position == -1) // Error
    {
        perror("get_user_pos() failed: ");
        return -1;
    } else if (found == false) // User NOT FOUND.. don't ask why
    {
        perror("add_user() tried to remove a non-existing user: ");
        return -1;
    }

    printf("REMOVING %s...\n", user.player.name);
    users[position].id = -2;
    --connected_users;
    return 0;
}

// Returns the users position in the array, if the user is not in the array
// returns the first array position found for insertion
// *NOTE* Adding a user with socket_fd -2 may cause undefined behavior
//
// Returns -1 on error
int get_user_pos(user_t user, bool *found)
{
    // Check if the user is in the array
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        if (user.id == users[i].id)
        {
            *found = true;
            return i;
        }
    }

    // If the user is not found, return the first available position
    *found = false;
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        if (users[i].id == -2) { return i; }
    }

    return -1; // Something went wrong
}

// Prints all active users
void print_users()
{
    pthread_mutex_lock(&mutex_lock);
    printf("-------------------");
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        if (users[i].id < 0) { continue; } // Not an active user

        printf("\nNAME: %s\nID: %d\nSOCKET: %d\nSTATUS: ", users[i].player.name, users[i].id, users[i].socket_fd);
        if (users[i].online)
        {
            printf("ONLINE\nACTION: ");
            print_action(users[i].action);
            printf("ACTION TOWARDS: %u\n", users[i].action_towards);
        }
        else { printf("OFFLINE\n"); }
        printf("ROOM: %d\n", users[i].player.room->id);
    }
    printf("-------------------\n");
    pthread_mutex_unlock(&mutex_lock);
}

// Prints the action passed to the function
void print_action(game_action_t action)
{
    switch (action)
    {
        case EXIT:
            printf("EXIT\n");
            break;
        case MOVE:
            printf("MOVE\n");
            break;
        case SHOOT:
            printf("SHOOT\n");
            break;
        case NOTHING:
            printf("NOTHING\n");
            break;
        default:
            break;
    }
}

// Checks if a user exists in the server's database
bool user_exists_database()
{
    // Search the database for that user (by name?)
    
    return false;
}

// Updates the game state based on each persons action
void update_game_state()
{
    pthread_mutex_lock(&mutex_lock);

    // Update every ongoing action
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        if (users[i].action == NOTHING) { continue; }

        // printf("%d\n", *users[i].player.room->attack_contents);

        if (users[i].action == MOVE)
        {
            if (!neighbour_found(*users[i].player.room, users[i].action_towards))
            {
                printf("Compared %u with %u from %s ", users[i].player.room->id, users[i].action_towards, users[i].player.name);
                printf(NO_PASSAGE_MSG);
                continue;
            }

            users[i].player.room = get_room(game_map, users[i].action_towards);
        } else if (users[i].action == SHOOT)
        {
            if (!neighbour_found(*users[i].player.room, users[i].action_towards))
            {
                printf(NO_PASSAGE_MSG);
                continue;
            }

            room_attack[i].id = users[i].action_towards;
            room_attack[i].attack = SHOOTING_ARROW;
        }

        users[i].action = NOTHING;
    }

    // Check the outcome of the previous loop
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        for (int j = 0; j < MAX_PLAYERS; ++j)
        {
            if (room_attack[j].id == users[i].player.room->id) // If the player is in the same room with an attack
            {
                printf("%s was hit with SHOOTING ARROW\n", users[i].player.name);

                // Remove the attack
                room_attack[j].attack = NO_ATTACK;
                room_attack[j].id = 0;
            }
        }

        break;
    }
    pthread_mutex_unlock(&mutex_lock);

    update_needed = false;
}
