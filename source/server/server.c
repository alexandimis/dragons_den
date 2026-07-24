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
#include "server.h"

// Global variables
int server_fd;
struct sockaddr_un addr;
int connected_users;
int id_count;
list_t *users;
bool update_needed = false;

pthread_mutex_t mutex_lock;

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

    while (1)
    {
        if (connected_users >= MAX_PLAYERS)
        {
            sleep(1);
            continue;
        }
        
        int client_fd = accept(server_fd, (struct sockaddr *)&addr, &addr_len);
        if (client_fd == -1)
        {
            perror("accept() failed: ");
            return NULL;
        }

        // Set the client socket to NON_BLOCKING (server doesn't wait for data that is not there)
        if(fcntl(client_fd, F_SETFL, fcntl(client_fd, F_GETFL) | O_NONBLOCK) < 0)
        {
            perror("fcntl() failed: ");
            continue;
        }

        pthread_mutex_lock(&mutex_lock);

        char player_name[NAME_SIZE];
        int bytes_read = recv(client_fd, player_name, sizeof(char) * NAME_SIZE, 0);
        if (bytes_read == -1)
        {
            perror("recv() failed: ");
            close(client_fd);
            return NULL;
        }
        
        ++connected_users;
        user_t new;
        new.socket_fd = client_fd;
        new.online = true;
        strcpy(new.player.name, player_name);
        new.id = id_count++;

        if (user_exists_database()) // Currently this will never go
        {
            // Handle that
        } else
        {
            new.player.arrows = INIT_ARROWS;
            new.player.room = malloc(sizeof(vertex_t));
            new.player.room->contents = EMPTY;
            new.player.room->id = new.id;
        }

        send(client_fd, &new.player.arrows, sizeof(new.player.arrows), 0);
        send(client_fd, &new.player.room->id, sizeof(new.player.room->id), 0);
        send(client_fd, &new.player.room->contents, sizeof(new.player.room->contents), 0);

        add_user(new);
        print_users();

        pthread_mutex_unlock(&mutex_lock);
    }

    return NULL;
}

// Every 5 seconds it scans the user list for offline users,
// if a user is offline he gets removed from the user list
void *user_disconnect(void *)
{
    while (1)
    {
        sleep(1);
        print_users();
        printf("Starting cleanup...\n");
        list_t *current = users;
        
        while (current != NULL)
        {
            if (!current->user.online)
            {
                pthread_mutex_lock(&mutex_lock);
                current = remove_user(current->user);
                --connected_users;
                pthread_mutex_unlock(&mutex_lock);
            }

            if (current == NULL) { break; }
            current = current->next;
        }
    }

    return NULL;
}

// Constantly listens for pings from users. If a user doesnt ping in a set time they are considered
// offline
void *ping_listen(void *)
{
    // while (1)
    // {
    //     // Listen for each users reply? If yes what should the timeout be...
    //     sleep(5);
    //     print_users();
    // }

    return NULL;
}

void *check_user_input(void *)
{
    while (1)
    {
        sleep(1);
        print_users();
        // Poll for input
        pthread_mutex_lock(&mutex_lock);
        list_t *current = users;
        

        while (current != NULL)
        {
            game_action_t action;

            if (recv(current->user.socket_fd, &action, sizeof(action), 0) > 0)
            {
                print_action(action);
                update_needed = true;

                if (action == EXIT) { current->user.online = false; }
            }

            current = current->next;
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
    // Game graph init
    graph_t *game_graph = NULL;
    while (1)
    {
        printf("Create new map (N) or load an existing one (L): ");
        char map_creation = 'a';
        scanf(" %c", &map_creation);
        if (map_creation == 'N') { game_graph = create_game_graph(LEVEL_FACTOR); break; } 
        else if (map_creation == 'L') { /* LOAD GRAPH FROM FILE */ break; }
        else { printf(WRONG_SELECTION_MSG); }
    }

    print_graph(game_graph);

    // Socket fd initialization
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) { return -1; }

    char *system_path = "/tmp/dragons_den.sock";
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, system_path);
    unlink(system_path);
    
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("bind() failed: ");
        return -1;
    }

    listen(server_fd, MAX_PLAYERS);

    // Thread creations
    users = NULL;
    connected_users = 0;
    id_count = 0;
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
            pthread_mutex_lock(&mutex_lock);
            printf("UPDATING!\n");
            update_needed = false;
            pthread_mutex_unlock(&mutex_lock);
        }
        // Send players new game state
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

void add_user(user_t user)
{
    // Struct initialization
    list_t *new = malloc(sizeof(list_t));
    if (new == NULL) 
    {
        perror("malloc() failed: ");
        return;
    }

    new->user.id = user.id;
    new->user.socket_fd = user.socket_fd;
    new->user.online = user.online;
    new->user.action = NOTHING;
    strcpy(new->user.player.name, user.player.name);
    new->previous = NULL;

    if (users == NULL)
    {
        new->next = NULL;
        users = new;
        return;
    }

    list_t *temp = users;
    new->next = temp;
    users = new;
}

// Returns the user after the removed one
list_t *remove_user(user_t user)
{
    printf("REMOVING %s...\n", user.player.name);
    list_t *current = find_user(user.id);

    if (current == NULL)
    {
        perror("find_user() tried to find an invalid id... failed to remove user: ");
        return NULL;
    }
    
    // If it's the first user in the list
    if (current->previous == NULL) { users = users->next; }
    else { current->previous->next = current->next; }
    
    // If it's not the last user in the list
    if (current->next != NULL) { current->next->previous = current->previous; }
    
    list_t *next = current->next;

    free(current);
    return next;
}

list_t *find_user(int id)
{
    list_t *current = users;

    while (current != NULL)
    {
        if (current->user.id == id) { return current; }
        current = current->next;
    }

    return NULL;
}

void print_users()
{
    list_t *current = users;

    printf("-------------------\n");
    while (current != NULL)
    {
        printf("printing %p and %p\n", current, current->next);
        printf("NAME: %s\nID: %d\nSOCKET: %d\nSTATUS: ", current->user.player.name, current->user.id, current->user.socket_fd);
        if (current->user.online)
        {
            printf("ONLINE\nACTION: ");
            print_action(current->user.action);
        }
        else { printf("OFFLINE\n"); }
        
        current = current->next;
    }
    printf("-------------------\n");
}

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

bool user_exists_database()
{
    // Search the database for that user (by name?)
    
    return false;
}

void update_game_state()
{
    if (!update_needed) { return; }

    list_t *current = users;

    while (current != NULL)
    {


        current = current->next;
    }
}