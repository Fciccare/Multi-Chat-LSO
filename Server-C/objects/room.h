#ifndef ROOM_H
#define ROOM_H

#define MAX_CLIENTS 32  // Max Clients supported by a room
#define MAX_CLIENTS_ZERO 256  // Max Clients supported by room zero, the starting room

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
void room_destroy(Room* r);

//set
void room_setId(Room* r, unsigned int id);
void room_setName(Room* r, const char* name);
void room_setClients_counter(Room* r, unsigned int clients_counter);
void room_setMaster_client(Room* r, Client* master_client);

//Other functions
Client* room_get_client_by_id(Room* r, int client_socket_id);
//TODO: DA SCRVERE ANCORA
void room_print(Room* r); //Debug function
char* room_to_string(Room* r);
bool room_add_client(Room* r, Client* client);
bool room_remove_client(Room* r, int socket_id); 

#endif