#ifndef SERVER_H
    #define SERVER_H

    #define MAX_PLAYERS 5
    #define AFK_TIMEOUT 180

    typedef struct {
        char name[NAME_SIZE];
        int user_fd;
        int socket_fd;
    } user_t;

    typedef struct list_t list_t;

    struct list_t {
        user_t user;
        list_t *next;
    };

    bool ping(user_t user);
#endif