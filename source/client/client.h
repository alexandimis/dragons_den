#ifndef CLIENT_H
    #define CLIENT_H

    extern int client_fd; // Extern so it can be used in the multiplayer game loop

    player_t server_connect(char *socket_path, player_t player);

#endif