#ifndef UTIL_H
#define UTIL_H

#define MAX_CLIENT 32  // Limite attuale

typedef struct{
    char* propic;
    char name[64];
    char password[256];
} User;

typedef struct{
    User user;
    int socket_id;
} Client;

typedef struct {
    char name[128];
    int online_client;
    Client* master_client;
    Client* client[MAX_CLIENT];
} Room;


//User function



//Client function



//Room function



#endif