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
void client_destory(Client* c);

//Set
void client_setUser(Client* c, User* user);
void client_setSocket_id(Client* c, int socket_id);
void client_setRoom_id(Client* c, unsigned int room_id);

//Print and Debug
char* client_to_string_full(Client* c); //Debug function
char* client_to_string(Client* c); //Debug function

#endif