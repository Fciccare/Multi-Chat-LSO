#ifndef ROOM_H
#define ROOM_H

#define MAX_CLIENTS 32  // Limite attuale di client supportati da una stanza

#include "client.h"

//Structure//

typedef struct {
    unsigned int id; 
    char* name;
    unsigned int clients_counter;
    Client* master_client;
    Client clients[MAX_CLIENTS];
} Room;


//Functions//

//Constructors and Drestory

Room* room_create(unsigned int id, const char* name, unsigned int clients_counter, Client* master_client);
void room_destroy(Room* r);

//set
void room_setId(Room* r, unsigned int id);
void room_setName(Room* r, const char* name);
void room_setClients_counter(Room* r, unsigned int clients_counter);
void room_setMaster_client(Room* r, Client* master_client);

//Other functions

/*DECISIONE DI DESIGN:
 *Mettere qui l'inizializzazione dell'array di puntatori a stanze?
 *Si potrebbe fare anche nel main (si però poi quanta robba là dentro ooooh?!?)
 *In ogni caso questo array DEVE essere una variabile globale perchè tipo tutti i thread ci devono accedere
 *Possibile problema di race condition ma quello si deve vedere testare fare
// void addRoom(Room*);
// void printRooms();
// void deleteRoom();
*/

void room_print(Room* r);
void room_add_client(Room* r, Client* client); //TO DO
void room_remove_client(Room* r, int socket_id); //TO DO

#endif