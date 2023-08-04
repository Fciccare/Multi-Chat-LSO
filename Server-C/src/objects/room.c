#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "room.h"
#include "client.h"

#include "../library/log.h"


//TESTARE PASSAGGIO DI PROPRIETA' DELLA STANZA

//POI TESTARE CANCELLAZIONE DELLE STANZE QUANDO TUTTI ESCONO

//Constructor and Drestory
Room* room_create(unsigned int id, const char* name, Client* master_client) {
  Room* r = (Room*)malloc(sizeof(Room));
  r->id = id;
  strcpy(r->name, name);
  r->clients_counter = 0;
  r->master_client = master_client;
  // log_debug("Setted id=%d, name=%s, client_counter=0, master_client", id, name);

  if(id == 0){ //Starting room
    r->clients = (Client**)malloc(sizeof(Client*) * MAX_CLIENTS_ZERO);
    bzero(r->clients, MAX_CLIENTS_ZERO);
    // log_debug("Created dinamic array %d size", MAX_CLIENTS_ZERO);
  }
  else{ //Regular room
    r->clients = (Client**)malloc(sizeof(Client*) * MAX_CLIENTS);
    bzero(r->clients, MAX_CLIENTS);
    // log_debug("Created dinamic array %d size", MAX_CLIENTS);
  }
  

  if(master_client != NULL){
    log_debug("Master client NOT NULL");
    room_add_client(r, master_client);
  }
  return r;
}

void room_delete(Room* r ){
  //Se la chiami senza che la stanza sia vuota potrebbero succedere casini, non farlo se possibile.

  if (r == NULL){ //unexpected behaviour
    log_error("Trying to delete NULL room");
    return;
  }

  log_debug("Deleting room: %d", r->id);
  if (r->clients_counter != 0){
    log_error("Deleting non-empty room, umpredictable behaviour, deleting it anyway.");
    room_clear(r); 
  }
  
  r->master_client = NULL;
  r->clients = NULL;
  room_destroy(r);
}

void room_destroy(Room* r) { 
  log_debug("Destroying room: %d", r->id);
  free(r);
  r = NULL; 
}


//Set
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


//Get
Client* room_get_client_by_id(Room* r, int client_socket_id){
  int online_client = r->clients_counter;
  int count = 0;

  //room_client_print(r); //debug

  int max = MAX_CLIENTS;
  if (r->id == 0)
    max = MAX_CLIENTS_ZERO;
  for (int i = 0; i < max; i++) {
    if(r->clients[i] != NULL){
      count++;
      if(r->clients[i]->socket_id == client_socket_id){
        // log_debug("found client with id:%d in room with id:%d", client_socket_id, r->id);
        return r->clients[i];
      }
    }
    if(count == online_client) {
      // log_debug("can't find client with id:%d in room with id:%d", client_socket_id, r->id);
      return NULL;
    }
  }
  return NULL;
}


//Logic
bool room_add_client(Room* r, Client* client) {
  if (r->id != 0 && r->clients_counter == MAX_CLIENTS) {
    log_warn("Room %d full, can't add %s", r->id, client_to_string(client));
    return false;
  } else if (r->id == 0 && r->clients_counter == MAX_CLIENTS_ZERO ){
    log_warn("Room zero full, can't add %s", client_to_string(client));
    return false;
  }

  Client** clients = r->clients;

  for(int i=0; i<MAX_CLIENTS; i++){
    if (clients[i] == NULL) { //found empty spot
      // log_debug("Adding %s to room: %d", client_to_string(client), r->id);

      r->clients[i] = client;
      r->clients_counter++;
      client->room_id = r->id;
      
      return true;
    }
  }
  //unexpected behaviour
  log_error("Could not find place in room %d for client %d", r->id, client->socket_id); 
  return false;
}

bool room_remove_client(Room* r, int socket_id) {
  if(r == NULL || r->clients_counter <= 0){ //unexpected behaviour
    log_error("Room null or Client count <=0 in room id: %d", r->id);
    return false;
  }

  Client** clients = r->clients;
  int online_clients = r->clients_counter;

  int max = MAX_CLIENTS;
  if(r->id==0)
    max = MAX_CLIENTS_ZERO;

  int count = 0;
  for (int i = 0; i < max; i++) {
    if (clients[i] != NULL) { 
      count++;
      if (clients[i]->socket_id == socket_id){ //Client Found
        r->clients_counter--;
        clients[i] = NULL;
        log_debug("Client with socket_id:%d removed from room:%d", socket_id, r->id);

        //If Client removed was master, chose a new master
        if(r->id != 0 && r->clients_counter > 0 && r->master_client->socket_id == socket_id) 
          room_change_master(r);
        
        return true;
      }
    }
    if (count == online_clients) return false;
  }
  return false;
}

bool room_change_master(Room* r){
  //Changes it to the first Client it finds in r->Clients

  Client** clients = r->clients;

  if(r == NULL || r->clients_counter <= 0){ //unexpected behaviour
    log_error("Room null or Client count <=0");
    return false;
  }

  int max = MAX_CLIENTS;

  for (int i = 0; i < max; i++) {
    if (clients[i] != NULL) {
      r->master_client = clients[i];
      log_debug("New Master is: %d", r->master_client->socket_id);
      return true;
    }
  }
  log_error("Could not change master");
  return false;
}

bool room_is_empty(Room* r){
  if(r->clients_counter == 0)
    return true;
  return false;
}

void room_clear(Room* r) {
  //Unexpected bheavior
  int max = MAX_CLIENTS;
  unsigned count = 0;
  for (int i = 0; i < max; i++) {
    if(r->clients[i] != NULL){
      count++;
      client_destroy(r->clients[i]);
    }
    if (count == r->clients_counter) break;
  }
  r->clients_counter = 0;
}

//Print and Debug
void room_print(Room* r) { //Debug funcion
  log_debug(room_to_string(r));
}

char* room_to_string(Room* r){
  if(r != NULL){
    char value[512];
    sprintf(value, "Room: {id: %d, name: %s, clients_counter: %d, Master%s}", r->id, r->name,r->clients_counter, client_to_string_full(r->master_client));
    return strdup(value);
  } 

  return "Client: NULL";
}

void room_client_print(Room* r){
  if(r!=NULL){
    for(int i=0; i < MAX_CLIENTS; ++i){
      if(r->clients[i] == NULL){
        printf("(%d): NULL \n", i);
      }else printf("(%d): NOT NULL \n", i);
    }
  }
}
