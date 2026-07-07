#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "server.h"
#include "../game/client.h"

int main(int argc, char **argv)
{
    // Handle arguments
    if (argc != 2)
    {
        perror("Incorrect number of arguments: ");
        return -1;
    }
    /* ADD ERROR CHECKING! */

    
    

    // Socket fd initialization
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) { return -1; }

    // Address struct initialization
    char *system_path = "/tmp/dragons_den.sock";
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, system_path);
    
    // Bind the socket fd to the socket adress, after ensuring its available
    unlink(system_path);
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("bind() failed: ");
        return -1;
    }

    // Listen for connections
    listen(server_fd, MAX_PLAYERS);

    // Accept connections
    int client_fd = 0;
    int party_size = accept_leader(server_fd, &client_fd);
    if (party_size == -1)
    {
        perror("Invalid party size: ");
        close(server_fd);
        unlink(system_path);
        return -1;
    }

    printf("Party size is %d\n", party_size);

    bool lobby_full = false;
    
    while (!lobby_full)
    {
        break;
    }
    
    
    // int buffer = 0, client_fd = 0, bytes_read = 0;

    // client_fd = accept(server_fd, NULL, NULL);
    // printf("New connection %d\n", client_fd);



    // do
    // {
    //     client_fd = accept(server_fd, NULL, NULL);
    //     printf("New connection %d\n", client_fd);

    //     // Read available data
    //     do 
    //     {
    //         bytes_read = recv(client_fd, &buffer, sizeof(int), 0);
    //         if (bytes_read == -1)
    //         {
    //             perror("recv() failed");
    //             break;
    //         }
            
    //     } while ((bytes_read > 0) && (buffer != -1));
    //     close(client_fd);

    // } while (buffer != -1);



    close(server_fd);
    unlink(system_path);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// Connect with the party leader and read the wanted party size
// Returns the number of members in the party
int accept_leader(int server_fd, int *client_fd)
{
    int party_size = -1;

    while (1)
    {
        *client_fd = accept(server_fd, NULL, NULL);
        int bytes_read = 0;

        // Read magic number
        char magic_number[64];
        do 
        {
            bytes_read = recv(*client_fd, magic_number, sizeof(char *), 0);
            if (bytes_read == -1)
            {
                perror("recv() failed");
                break;
            }
            
        } while ((bytes_read > 0) && (party_size != -1));

        if (strncmp(magic_number, PARTY_HOST_MAGIC_NUMBER, strlen(PARTY_HOST_MAGIC_NUMBER)) != 0)
        {
            printf("%s\n", magic_number);

            continue;
        }
    }


    
    printf("New connection %d\n", *client_fd);

    

    

    return party_size;
}