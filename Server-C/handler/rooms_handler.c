#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "rooms_handler.h"
#include "../library/log.h"

Room* rooms[MAX_ROOMS] = {0};
unsigned int rooms_active = 1; //room 0 (starting room) is always active
unsigned int next_unactive_room_index = 1; //next empty spot to fill with new room(always updated!)
//0 must never be used as an id!



/*TODO: (VAI VALE U CAN DO IT (si me lo dico da sola))
- terminare (chiamare dal main e poi dal socket_dispatcher) caso di disconnessione
- logica per uscire normalmente dalla app (dalla room 0)
- logica per uscire normalmente dalla app da qualsiasi stanza (dobbiamo vedere da android se si può fare facile)

- Se client che esce è master, dare master a qualcun altro
- Se però master è ultimo, cancellare stanza

- rooms_destory()



*/




void init_starting_room(){
  rooms[0] = room_create(0, "Defualt Room", NULL);
}

/*TODO: rooms_destroy(){
  //for each room, call room_destroy();

}*/


//getters
Room* rooms_get_room_by_id(unsigned int room_id){

  if(room_id >= MAX_ROOMS){
    log_error("Error: rooms_get_room_by_id, out of bounds array index");
    return NULL;
  }

  return rooms[room_id];
}

Client* rooms_get_client_by_id(int room_id, int client_socket_id){
  return room_get_client_by_id(rooms[room_id], client_socket_id);
}

void rooms_get_formatted_room(int i, char* buff) { //Get a specific room in a formatted manner 
  if (rooms[i] == NULL) //If room is not active, returns terminatoin character
    buff[0] = '\0';
  else
    sprintf(buff, "%d<>%s<>%d\n",i, rooms[i]->name, rooms[i]->clients_counter); // roomID<>roomNAME<>clientsConnected
}

bool is_valid_room_id(int id) {
  return (0 <= id < MAX_ROOMS);
}


//logic
void update_next_unactive_room_index() { //to be called after every rooms_add_room

  if(rooms_active >= MAX_ROOMS){ //unexpected behaviour
    log_error("trying to find next_unactive_room_index when there isn't one");
    return;
  }

  if (next_unactive_room_index == 0) //unexpected behaviour
    next_unactive_room_index++;

  unsigned int i = next_unactive_room_index + 1;
  while (true) { 
    if (rooms[i] != NULL) {
      if(i++ == MAX_ROOMS) //if we get out of the array
        i=1;               //we keep searching from the start (0 excluded)
        
    } else {//rooms[i]==NULL
      log_debug("next_unactive_room_index:%d", i);
      next_unactive_room_index = i;
      return;
    }

    if(next_unactive_room_index == i){//unexpected behaviour: we looked the entire array, didn't find NULL in any
      log_error("no NULL elements in rooms array");
      return;
    }
  }
}

bool rooms_add_room(Room* new_room) {

  if(new_room == NULL){ //unexpected behaviour
    log_error("Error: Trying to add null room to rooms array");
    return false;
  }

  if (rooms_active == MAX_ROOMS){ //if there is no space for a new room
    log_warn("Max Room reached, unable to create new room");
    return false;
  }
  
  log_debug("Next unactive room index that will be used: %d", next_unactive_room_index);
  rooms[next_unactive_room_index] = new_room;
  rooms_active++;
  new_room->id=next_unactive_room_index;
  new_room->master_client->room_id=next_unactive_room_index;
  log_debug("New room assegned to array, set new room id, set master client room id, increment rooms_active counter");
  
  if(rooms_active<MAX_ROOMS)
    update_next_unactive_room_index();
  return true;
}

void delete_room(unsigned int room_id) {
  if(room_id == 0 || room_id >= MAX_ROOMS || rooms[room_id] == NULL) { //unexpected behaviour
    log_error("Trying to delete non-existing room (room_id:%d)", room_id);
    return;
  }

  for (int i = 0; i < MAX_CLIENTS; i++)  {
    Client* client_to_move = rooms[room_id]->clients[i];
    if(client_to_move != NULL) {
      rooms_move_to_zero(client_to_move, room_id);
    }
  }
  
  log_debug("deleting room: %s",room_to_string(rooms[room_id]));
  room_delete(rooms[room_id]); 
  rooms[room_id] = NULL;
  rooms_active--;
  log_debug("Room deleted, number of active rooms:%d", rooms_active);

  if(room_id<next_unactive_room_index)
    next_unactive_room_index = room_id;
  log_debug("next_unactive_room_index:%d", next_unactive_room_index);
}

bool rooms_remove_from_zero(int socket_id){ //removes a client from starting room
  Room* room_zero = rooms_get_room_by_id(0);
  log_debug("Removing client with socket_it %d from starting room", socket_id);
  return room_remove_client(room_zero, socket_id);
}

bool rooms_move_to_zero(Client* client, int old_room_id){ //removes from current room and moves to room zero

  if(!is_valid_room_id(old_room_id)){ //unexpected behaviour
    log_error("Trying to access room with invalid id:%d", old_room_id);
    return false;
  }

  if(client == NULL) { //unexpected behaviour
    log_error("Trting to move NULL client from room:%d", old_room_id);
  }

  Room* room_zero = rooms_get_room_by_id(0);
  bool status = room_add_client(room_zero, client);
  if(status){
    status = room_remove_client(rooms[old_room_id], client->socket_id);
    log_debug("Moved client %s to starting room", client_to_string(client));
  } 
  return status;
}

  //NEED TO TEST!!
void rooms_delete_client(int socket_id) { 
  //socket_id is supposed to be a valid socket_id
  int max = rooms_active;
  Client* c = NULL;
  for (int i = 0; i < MAX_ROOMS; i++){
    if (rooms[i] != NULL) {
      if ((c = rooms_get_client_by_id(i, socket_id)) != NULL) {
        rooms_remove_from_and_destory(rooms[i], c);
      }
    }
    if (i == max) break;
  }
  //unexpected behaviour
  log_error("Did not find socket_id %d", socket_id);
}

  //NEED TO TEST!!
void rooms_delete_client_from_room(int socket_id, int room_id) {
  if((!is_valid_room_id(room_id)) || (rooms[room_id] == NULL)){ //unexpected behaviour
    log_error("Trying to delete client with socket_id %d from invalid room id %d", socket_id, room_id);
    return;
  }

  Client* c = NULL;
  if((c = room_get_client_by_id(rooms[room_id], socket_id)) != NULL){
    rooms_remove_from_and_destory(rooms[room_id], c);
  } else { //unexpected behaviour
    log_error("Trying to delete client n.%d from room n.%d, but it is not there!", socket_id, room_id);
  }

}

void rooms_remove_from_and_destory(Room* r, Client* c){
  //check if r and c NOT NULL before calling this function!
  int socket_id = c->socket_id;
  room_remove_client(r, c->socket_id);
  log_debug("Destroying %s found in room %d", client_to_string(c), r->id);
  client_destory(c);
}

void print_rooms() { //debug function
  for(int i = 0; i<MAX_CLIENTS; i++) {
    if(rooms[i] != NULL)
      room_print(rooms[i]);
    else
      printf("%d : NULL ROOM\n", i);
  }
}



