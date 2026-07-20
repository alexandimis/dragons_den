#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>

#include "../game/game.h"
#include "server.h"

// Global variables
int server_fd;
struct sockaddr_un addr;
int connected_users;
int id_count;
list_t *users;

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
        int client_fd = accept(server_fd, (struct sockaddr *)&addr, &addr_len);
        if (client_fd == -1)
        {
            perror("accept() failed: ");
            return NULL;
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
        new.online = true;
        strcpy(new.name, player_name);
        new.socket_fd = client_fd;
        new.id = id_count++;
        add_user(new);
        print_users();

        pthread_mutex_unlock(&mutex_lock);
    }

    return NULL;
}

// Whenever a user disconnects, remove them from the user list
void *user_disconnect(void *)
{
    while (1)
    {
        // No need for constant checking if someone is offline
        // sleep(AFK_TIMEOUT);

        sleep(5);
        printf("USERS IS %p\n", users);

        if (users == NULL) { continue; } // No online users
        list_t *current = users; 
        
        while (current != NULL)
        {
            // printf("IM AT %p -> %s\n", current, current->user.name);

            if (current->user.online) // Is online
            {
                current = current->next;
                continue;
            }

            // Is offline
            pthread_mutex_lock(&mutex_lock);

            user_t user_to_remove = current->user;
            current = remove_user(user_to_remove);

            pthread_mutex_unlock(&mutex_lock);

            printf("I AM RETURNED %p\n", current);
            if (current == NULL)
            {
                break;
            }
        }
    }

    return NULL;
}

void *ping_users(void *)
{
    while (1)
    {
        // Ping every user in the list

        // Listen for each users reply? If yes what should the timeout be...
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
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

    // Create threads to handle connections
    users = NULL;
    connected_users = 0;
    id_count = 0;
    pthread_t connect_thread;
    pthread_create(&connect_thread, NULL, user_connect, NULL);
    pthread_t disconnect_thread;
    pthread_create(&disconnect_thread, NULL, user_disconnect, 0);
    pthread_t pinger;
    pthread_create(&pinger, NULL, ping_users, NULL);

    bool running = true;
    while (running)
    {

    }

    pthread_join(connect_thread, NULL);
    pthread_join(disconnect_thread, NULL);
    close(server_fd);
    unlink(system_path);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// Pings a user.. Returns true if the user pings back (is online), false if doesn't until a timeout value (offline)
bool ping(user_t user)
{
    bool online = false;
    
    // Ping
    
    // Wait for ping

    return online;
}

void add_user(user_t user)
{
    list_t *new = malloc(sizeof(list_t));
    new->user.id = user.id;
    new->user.socket_fd = user.socket_fd;
    strcpy(new->user.name, user.name);
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
    list_t *current = find_user(user.id);

    if (current == NULL)
    {
        perror("find_user() tried to find an invalid id... failed to remove user: ");
        return NULL;
    }

    if (current->previous != NULL) // If it's not the first user in the list
    {
        current->previous->next = current->next;
    } else
    {
        users = users->next;
    }
    
    if (current->next != NULL) // If it's not the last user in the list
    {
        current->next->previous = current->previous;
    }
    
    list_t *next = current->next;

    free(current);
    return next;
}

list_t *find_user(int id)
{
    list_t *current = users;

    while (current != NULL)
    {
        if (current->user.id == id)
        {
            return current;
        }

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
        printf("NAME: %s\nID: %d\n SOCKET: %d\n", current->user.name, current->user.id, current->user.socket_fd);
        current = current->next;
    }
    printf("-------------------\n");
}