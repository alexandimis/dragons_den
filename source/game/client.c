#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "client.h"

// Connect to the server
int server_connect_host(char *socket_path)
{
    struct sockaddr_un addr;
    int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socket_path);

    int error = connect(client_fd, (struct sockaddr *)&addr, sizeof(addr));

    return error;
}