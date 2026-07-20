#ifndef SERVER_H
    #define SERVER_H

    #define MAX_PLAYERS 5
    #define AFK_TIMEOUT 10

    typedef struct {
        char name[NAME_SIZE];
        int id;
        int socket_fd;
    } user_t;

    typedef struct list_t list_t;

    struct list_t {
        user_t user;
        list_t *previous;
        list_t *next;
    };

    // Functions
    bool ping(user_t user);
    void add_user(user_t user);
    void remove_user(user_t user);
    list_t *find_user(int id);
    void print_users();
#endif