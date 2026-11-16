#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "game.h"
#include "client.h"

struct sockaddr_un addr;
int client_fd;

// Connect to the server, and init the player struct
// On error player.arrows will be -1
player_t server_connect(char *socket_path, player_t player)
{
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1)
    {
        player.arrows = -1;
        perror("socket() failed: ");
        return player;
    }

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socket_path);

    if (connect(client_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        player.arrows = -1;
        perror("connect() failed: ");
        close(client_fd);
        return player;
    }

    // Send player name to the server
    send(client_fd, &player.name, sizeof(char) * NAME_SIZE, 0);
    player.arrows = 0;
    strcpy(player.name, "alex");
    player.room = NULL;

    return player;
}

