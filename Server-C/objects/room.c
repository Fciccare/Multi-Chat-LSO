#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "user.h"
#include "client.h"
#include "room.h"

//Constructors and Drestory

Room* room_create(unsigned int id, const char* name, Client* master_client) {
  Room* r = (Room*)malloc(sizeof(Room));
  r->id = id;
  strcpy(r->name, name);
  r->clients_counter = 0;
  r->master_client = master_client;
  if(id == 0)
    r->clients = (Client**)malloc(sizeof(Client*) * MAX_CLIENTS_ZERO);  // Dinamico 256 POSTI
  else 
    r->clients = (Client**)malloc(sizeof(Client*) * MAX_CLIENTS);  // Dinamico 32 POSTI

  if(master_client != NULL)
    room_add_client(r, master_client);
  return r;
}

void room_destroy(Room* r) {
  free(r);
}

//set

void room_setId(Room* r, unsigned int id) {
  r->id = id;
}

void room_setName(Room* r, const char* name) {
  strcpy(r->name,name);
}

void room_setClients_counter(Room* r, unsigned int clients_counter){
  r->clients_counter = clients_counter;
}

void room_setMaster_client(Room* r, Client* master_client) {
  r->master_client = master_client;
}

Client* room_get_user_by_id(Room* r, int client_socket_id){
  int online_client = r->clients_counter;
  int count = 0;
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if(r->clients[i] != NULL){
      count++;
      if(r->clients[i]->socket_id == client_socket_id)
        return r->clients[i];
    }
    if(count == online_client)
      return NULL;
  }
}

//Other funcions

void room_print(Room* r) {
  printf("---\nid = %d\nname = %s\nclients_counter = %d\nmaster_client:\n", r->id, r->name,r->clients_counter);
  if(r->master_client!=NULL)
    client_print(r->master_client);

  // for(int i = 0; i < r->clients_counter; ++i){
  //   printf("(%d) : ", i);
  //   if(r->clients[i] == NULL)
  //     printf("NULL\n");
  //   else 
  //     printf("NOT NULL\n");
  // }

  printf("Per ora ometto la stampa della lista dei client\n---\n");
}

bool room_add_client(Room* r, Client* client) {
  if (r->id != 0 && r->clients_counter == MAX_CLIENTS) {
    return false;
  } else if (r->id == 0 && r->clients_counter == MAX_CLIENTS_ZERO ){
    return false;
  }
  int index = r->clients_counter;
  r->clients[index] = client; 
  r->clients_counter = index+1;

  return true;
}

void room_remove_client(Room* r, int socket_id) {
  //TO DO ricerca lineare del Client e rimozione dall'Array Clients
  if(r->clients_counter>0)
    r->clients_counter--;
  printf("\nHo cancellato (per finta, ho solo derementato il contatore) il client\n");
}