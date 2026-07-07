#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "server.h"

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
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, argv[1]);
    
    // Bind the socket fd to the socket adress
    system("rm /tmp/dragons_den.sock"); // WARNING!! If the socket address is changed, this has to be changed too

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("bind() failed: ");
        return -1;
    }

    // Listen for connections
    listen(server_fd, MAX_PLAYERS);

    // Accept connections
    bool lobby_full = false;
    
    while (!lobby_full)
    {
        
    }
    
    
    int buffer = 0, client_fd = 0, bytes_read = 0;
    client_fd = accept(server_fd, NULL, NULL);
    printf("New connection %d\n", client_fd);



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
    unlink(argv[1]); // Maybe change argv[1]

    return 0;
}