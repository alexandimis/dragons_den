#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>

#include "server.h"

int server_fd;
struct sockaddr_un addr;

// *NOTE* maybe it could be a user list instead of an array...
// Whenever a user connects to the server, add them to the user array (probably a dynamic array of pointers to user structs)
// Whenever a user disconnects, remove them from the array and rearrange it
void *user_connection_handling(void *)
{
    socklen_t addr_len = sizeof(addr);

    while (1)
    {
        accept(server_fd, (struct sockaddr *)&addr, &addr_len);
        printf("NEW CONNECTION!\n");
    }

    return NULL;
}

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

    // Create a thread to handle connections
    pthread_t user_connection_handling_thread;
    pthread_create(&user_connection_handling_thread, NULL, user_connection_handling, NULL);

    

    // Accept connections
    // int client_fd[MAX_PLAYERS] = {};
    // int connected_players = 0;
    
    bool running = true;
    while (running)
    {

    }

    pthread_join(user_connection_handling_thread, NULL);
    close(server_fd);
    unlink(system_path);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////