#ifndef ROOM_H
#define ROOM_H

#define MAX_CLIENTS 32  // Limite attuale di client supportati da una stanza
#define MAX_CLIENTS_ZERO 256  // Limite attuale di client supportati da una stanza

#include "client.h"

//Structure//

typedef struct {
    unsigned int id; 
    char name[32];
    unsigned int clients_counter;
    Client* master_client;
    Client** clients;
} Room;


//Functions//

//Constructors and Drestory

Room* room_create(unsigned int id, const char* name, Client* master_client);
void room_destroy(Room* r);

//set
void room_setId(Room* r, unsigned int id);
void room_setName(Room* r, const char* name);
void room_setClients_counter(Room* r, unsigned int clients_counter);
void room_setMaster_client(Room* r, Client* master_client);

//Other functions
void room_print(Room* r);
bool room_add_client(Room* r, Client* client); //TO DO
void room_remove_client(Room* r, int socket_id); //TO DO

#endif