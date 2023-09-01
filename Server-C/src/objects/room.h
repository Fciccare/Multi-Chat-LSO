#ifndef ROOM_H
#define ROOM_H

#define MAX_CLIENTS 32  // Max Clients supported by a room
#define MAX_CLIENTS_ZERO 256  // Max Clients supported by room zero, the starting room

#include <pthread.h>
#include "client.h"

//Structure//

typedef struct {
    unsigned int id; 
    char name[32];
    unsigned int clients_counter;
    Client* master_client;
    Client** clients; //Clients connected to the room
} Room;


//Functions//

//Constructor and Drestory
Room* room_create(unsigned int id, const char* name, Client* master_client);
void room_delete(Room* r);
void room_destroy(Room* r);

//Get
Client* room_get_client_by_id(Room* r, int client_socket_id);

//Logic
bool room_add_client(Room* r, Client* client);
int room_remove_client(Room* r, int socket_id); 
int room_change_master(Room* r);
bool room_is_empty(Room* r);
void room_clear(Room* r);


#endif