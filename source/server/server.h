#ifndef SERVER_H
    #define SERVER_H

    #define MAX_PLAYERS 5
    #define AFK_TIMEOUT 10
    #define LEVEL_FACTOR 0

    typedef struct {
        player_t player;
        bool online; // Whenever a user goes offline, "online" becomes false
        int id;
        int socket_fd;
        game_action_t action;
    } user_t;

    typedef struct list_t list_t;

    struct list_t {
        user_t user;
        list_t *previous;
        list_t *next;
    };

    // Functions
    void add_user(user_t user);
    list_t *remove_user(user_t user);
    list_t *find_user(int id);
    void print_users();
    void print_action(game_action_t action);
    bool user_exists_database();

    void update_game_state();
#endif