#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "room.h"
#include "client.h"

#include "../library/log.h"

//Constructor and Drestory

Room* room_create(unsigned int id, const char* name, Client* master_client) {
  Room* r = (Room*)malloc(sizeof(Room));
  log_debug("Room malloc successful");
  r->id = id;
  strcpy(r->name, name);
  r->clients_counter = 0;
  r->master_client = master_client;
  log_debug("Setted id=%d, name=%s, client_counter=0, master_client", id, name);

  if(id == 0){ //Starting room
    r->clients = (Client**)malloc(sizeof(Client*) * MAX_CLIENTS_ZERO);
    log_debug("Created dinamic array %d size", MAX_CLIENTS_ZERO);
  }
  else{ //Regular room
    r->clients = (Client**)malloc(sizeof(Client*) * MAX_CLIENTS);
    log_debug("Created dinamic array %d size", MAX_CLIENTS);
  }

  if(master_client != NULL){
    log_debug("Master client NOT NULL");
    room_add_client(r, master_client);
  }
  return r;
}

void room_destroy(Room* r) {
  log_debug("destroying room");
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

//Other funcions

Client* room_get_client_by_id(Room* r, int client_socket_id){
  int online_client = r->clients_counter;
  int count = 0;
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if(r->clients[i] != NULL){
      count++;
      if(r->clients[i]->socket_id == client_socket_id){
        log_debug("found client with id:%d in room with id:%d", client_socket_id, r->id);
        return r->clients[i];
      }
    }
    if(count == online_client) {
      log_debug("can't find client with id:%d in room with id:%d", client_socket_id, r->id);
      return NULL;
    }
  }
}

void room_print(Room* r) { //Debug funcion
  log_debug("Room: {id: %d, name: %s, clients_counter: %d, Master%s}", r->id, r->name,r->clients_counter, client_to_string_full(r->master_client));
}

char* room_to_string(Room* r){
  //TODO: DA SCRIVER!
  return NULL;
}

bool room_add_client(Room* r, Client* client) { //Non cicla, please fix it ❤
  //TODO: RICONTROLLAREEEEEEEEEEEEEEEEE
  if (r->id != 0 && r->clients_counter == MAX_CLIENTS) {
    return false;
  } else if (r->id == 0 && r->clients_counter == MAX_CLIENTS_ZERO ){
    return false;
  }
  int index = r->clients_counter;
  r->clients[index] = client; 
  r->clients_counter = index+1;

  //Remove from room zero
  client->room_id = r->id;
  return true;
}

bool room_remove_client(Room* r, int socket_id) {
  //TODO: ricerca lineare del Client e rimozione dall'Array Clients
  if(r== NULL || r->clients_counter<=0){
    printf("Room null or Client count <=0 in room id: %d\n", r->id);
    return false;
  }
  Client** clients = r->clients;
  int online_client = r->clients_counter;
  int max = MAX_CLIENTS;
  if(r->id==0)
    max = MAX_CLIENTS_ZERO;
  int count = 0;
  for (int i = 0; i < max; i++) {
    if (*(clients+i) != NULL) {
      count++;
      if ((*(clients+i))->socket_id == socket_id){
        r->clients_counter = r->clients_counter-1;
        *(clients+i) = NULL;
        return true;
      }    
    }
    if (count == online_client) return false;
  }
  //BUGIA
  printf("\nHo cancellato (per finta, ho solo derementato il contatore) il client\n");
}