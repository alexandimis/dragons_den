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
        strcpy(new.name, player_name);
        new.socket_fd = client_fd;
        new.id = id_count++;
        add_user(new);

        print_users();
    }

    return NULL;
}

// Whenever a user disconnects, remove them from the user list
void *user_disconnect(void *)
{
    while (1)
    {
        // No need for constant checking if someone is offline
        sleep(AFK_TIMEOUT);
        list_t *current = users->next; 
        
        while (current != NULL)
        {
            if (ping(current->user)) // Is online
            {
                current = current->next;
                continue;
            }

            // Is offline
            remove_user(current->user);
            current = current->next;
        }
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

    // Create 2 threads to handle connections
    connected_users = 0;
    id_count = 0;
    pthread_t connect_thread;
    pthread_create(&connect_thread, NULL, user_connect, NULL);

    users = NULL;

    pthread_t disconnect_thread;
    pthread_create(&disconnect_thread, NULL, user_disconnect, 0);
    
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

void remove_user(user_t user)
{
    list_t *current = find_user(user.id);

    if (current == NULL)
    {
        perror("find_user() tried to find an invalid id... failed to remove user: ");
        return;
    }

    current->previous->next = current->next;
    current->next->previous = current->previous;
    free(current);
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