#ifndef SERVER_H
    #define SERVER_H

    #define MAX_PLAYERS 5
    #define AFK_TIMEOUT 10
    #define LEVEL_FACTOR 0

    typedef struct {
        player_t player;
        bool online; // Whenever a user goes offline, "online" becomes false
        int id; // Set to -1 to indicate a NOT YET ADDED user / Set to -2 to indicate an empty array spot
        int socket_fd;
        game_action_t action;
        unsigned int action_towards;
    } user_t;

    typedef struct list_t list_t;

    struct list_t {
        user_t user;
        list_t *previous;
        list_t *next;
    };

    // Functions
    void add_user(user_t user);
    int remove_user(user_t user);
    int get_user_pos(user_t user, bool *found);

    void print_users();
    void print_action(game_action_t action);
    
    bool user_exists_database();

    void update_game_state();
#endif