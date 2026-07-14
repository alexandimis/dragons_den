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
list_t users;

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
        // Add player to the user list

        printf("THE NAME IS %s..from %d bytes\n", player_name, bytes_read);
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
        list_t *current = users.next; 
        
        while (current != NULL)
        {
            if (ping(current->user)) // Is online
            {
                current = current->next;
                continue;
            }

            // Is offline
            
            // REMOVE USER
            current = current->next
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
    pthread_t connect_thread;
    pthread_create(&connect_thread, NULL, user_connect, NULL);

    users.next = NULL;
    user_t head;
    strcpy(head.name, "HEAD_OF_LIST");
    head.socket_fd = -1;
    head.user_fd = -1;
    users.user = head;

    pthread_t disconnect_thread;
    pthread_create(&disconnect_thread, NULL, user_disconnect, 0);
    
    bool running = true;
    while (running)
    {

    }

    pthread_join(user_connect, NULL);
    pthread_join(user_disconnect, NULL);
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
    bool online = true;

    // Ping
    
    // Wait for ping

    return online;
}