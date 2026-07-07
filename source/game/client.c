#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "client.h"

int client_fd;
struct sockaddr_un addr;

// Connect to the server
int server_connect_host(char *socket_path)
{
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socket_path);

    int error = connect(client_fd, (struct sockaddr *)&addr, sizeof(addr));

    return error;
}

// Send the server data 
int server_send_data(int data, char *magic_number)
{
    if (send(client_fd, magic_number, sizeof(magic_number), 0) == -1)
    {
        perror("sendto() failed to send data: ");
        return -1;
    }

    if (send(client_fd, &data, sizeof(data), 0) == -1)
    {
        perror("sendto() failed to send data: ");
        return -1;
    }

    return 0;
}