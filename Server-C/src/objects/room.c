#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "room.h"
#include "client.h"

#include "../library/log.h"

//NOTICE: all the functions must be called with the corresponding room mutex locked
//        and the room must be unlocked after the function call
//        bad things will happen otherwise
//        you were warned

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
    memset(r->clients, 0, sizeof(Client*) * MAX_CLIENTS_ZERO);
    // log_debug("Created dinamic array %d size", MAX_CLIENTS_ZERO);
    r->master_client = NULL; //Starting room has no master client
    return r;
  }
  //Regular room
  else{ 
    r->clients = (Client**)malloc(sizeof(Client*) * MAX_CLIENTS);
    memset(r->clients, 0, sizeof(Client*) * MAX_CLIENTS);
    // log_debug("Created dinamic array %d size", MAX_CLIENTS);
  }

  if(master_client != NULL) {
    // log_debug("Master client NOT NULL");
    room_add_client(r, master_client);
  }
  else { //Unexpected behaviour
    log_error("Master client NULL");
  }
  return r;
}

void room_delete(Room* r ){
  //NOTICE: do not call this function if the room is not empty, it will cause unexpected behaviour

  if (r == NULL){ //unexpected behaviour
    log_error("Trying to delete NULL room");
    return;
  }

  log_debug("Deleting room: %d", r->id);
  if (r->clients_counter != 0){
    log_error("Deleting non-empty room, umpredictable behaviour (and memory leak), deleting it anyway.");
    room_clear(r); 
  }
  
  log_debug("Destroying room: %d", r->id);
  r->master_client = NULL;
  r->clients = NULL;
  room_destroy(r);
}

void room_destroy(Room* r) { //Called by room_delete
  
  // log_debug("Destroying room %d", r->id);
  // free(r->clients);
  // r->clients = NULL; 
  free(r);
  r = NULL;
  // log_debug("Room destroyed");
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

  if (client == NULL) {
    log_error("Trying to add NULL client to room");
    return false;
  }

  if(r == NULL){
    log_warn("Trying to add client to NULL room");
    return false;
  }

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

int room_remove_client(Room* r, int socket_id) {

  //Returns: -2 on error, -1 if room was deleted, room_id if master changed, 0 if client removed but master not changed
  
  if(r == NULL || r->clients_counter <= 0){ //unexpected behaviour
    log_error("Room null or Client count <=0 in room id: %d", r->id);
    return -2;
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
        // log_debug("Client with socket_id:%d removed from room:%d", socket_id, r->id);

        if(r->id == 0)
          return 0;

        if(r->clients_counter == 0 && r->id != 0) { //Check if empty, if true delete the room (but never delete room 0)
          // log_debug("Room %d is empty, destroying it", r->id);
          room_delete(r);
          return -1;
        }

        //If Client removed was master, choose a new master
        if(r->id != 0 && r->master_client->socket_id == socket_id)
          return room_change_master(r);

        return 0; //Client removed but master not changed
      }
    }
    if (count == online_clients) return -2; //unexpected behaviour
  }
  return -2; //unexpected behaviour
}

int room_change_master(Room* r){
  //Changes it to the first Client it finds in r->Clients
  //Retuns: -2 on error, room_id on success [Never -1]

  Client** clients = r->clients;

  if(r == NULL || r->clients_counter <= 0){ //unexpected behaviour
    log_error("Room null or Client count <=0");
    return -2;
  }

  int max = MAX_CLIENTS;

  for (int i = 0; i < max; i++) {
    if (clients[i] != NULL) {
      r->master_client = clients[i];
      // log_debug("New Master is: %d", r->master_client->socket_id);
      return r->id;
    }
  }
  
  //unexpected behaviour
  log_error("Could not change master");
  return -2;
}

bool room_is_empty(Room* r){ //to be called inside mutex
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
      r->clients[i] = NULL;
    }
    if (count == r->clients_counter) break;
  }
  r->clients_counter = 0;
}
