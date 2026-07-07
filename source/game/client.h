#ifndef CLIENT_H
    #define CLIENT_H

    #define PARTY_HOST_MAGIC_NUMBER "IAmYourHost"
    #define PARTY_MEMBER_MAGIC_NUMBER "IAmNotYourHost"
    int server_connect_host(char *socket_path);
    int server_send_data(int data, char *magic_number);
#endif