#ifndef CLIENT_H
#define CLIENT_H

#include "user.h"
#include "client.h"


//Structure//

typedef struct{
    User* user;
    int socket_id;
    unsigned int room_id;
} Client;

//Functions//

//Constructors and Destroy
Client* client_create(User* user, int socket_id, unsigned int room_id);
void client_destroy(Client* c);

//Debug
char* client_to_string_full(Client* c);
char* client_to_string(Client* c);

#endif