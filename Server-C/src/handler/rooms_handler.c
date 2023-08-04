#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "rooms_handler.h"
#include "../library/log.h"

Room* rooms[MAX_ROOMS] = {0};
unsigned int rooms_active = 1; //room 0 (starting room) is always active
unsigned int next_unactive_room_index = 1; //next empty spot to fill with new room(always updated!)
//NOTICE: 0 must never be used as an id!



/*TODO: (VAI VALE U CAN DO IT (si me lo dico da sola))

- rooms_destory() (Per quando si termina il server)
  DA FARE ANCORA

- (dobbiamo vedere da android se si può fare facile)
  Logica per uscire normalmente dalla app da qualsiasi stanza

*/



//Init and Destroy
void init_starting_room(){
  rooms[0] = room_create(0, "Defualt Room", NULL);
  log_info("Default room initiated, SERVER READY");
}

/*TODO: rooms_destroy(){ FUNCTION TO CALL WHEN CLOSING SERVER
  //for each room, call room_destroy();

}*/


//Get
Room* rooms_get_room_by_id(unsigned int room_id){

  if(room_id >= MAX_ROOMS){
    log_error("Error: rooms_get_room_by_id, out of bounds array index");
    return NULL;
  }

  return rooms[room_id];
}

Client* rooms_get_client_from_room_by_id(int room_id, int client_socket_id){
  return room_get_client_by_id(rooms[room_id], client_socket_id);
}

Client* rooms_get_client_by_id(int socket_id){
  Client* c = NULL;
  int max = rooms_active;
  for (int i = 0; i < MAX_ROOMS; i++){
    if (rooms[i] != NULL) {
      if ((c = rooms_get_client_from_room_by_id(i, socket_id)) != NULL)
        break;
    }
    if (i == max) 
      break;
  }
  return c;
}

void rooms_get_formatted_room(int i, char* buff) { //Get specific room in formatted manner 
  if (rooms[i] == NULL) //If room is not active, returns terminatoin character
    buff[0] = '\0';
  else
    sprintf(buff, "%d<>%s<>%d\n",i, rooms[i]->name, rooms[i]->clients_counter); // roomID<>roomNAME<>clientsConnected
}

bool is_valid_room_id(int id) {
  //return (0 <= id < MAX_ROOMS);
  return (0<= id && id < MAX_ROOMS) ? true : false;
}


//Rooms Logic
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

void rooms_delete_room(unsigned int room_id) {
  Room* room = rooms[room_id];
  if(room_id == 0 || room_id >= MAX_ROOMS || room == NULL) { //unexpected behaviour
    log_error("Trying to delete non-existing room (room_id:%d)", room_id);
    return;
  }else log_debug("Room exists, deleting...");

  if(!(room_is_empty(room))){
    for (int i = 0; i < MAX_CLIENTS; i++)  {
        Client* client_to_move = room->clients[i];
        if(client_to_move != NULL) {
          rooms_move_to_zero(client_to_move, room_id);
        }
      }
  }
  
  log_debug("deleting room: %s", room_to_string(rooms[room_id]));
  room_delete(rooms[room_id]); 
  rooms[room_id] = NULL;
  rooms_active--;
  log_debug("Room deleted, number of active rooms:%d", rooms_active);

  if(room_id<next_unactive_room_index)
    next_unactive_room_index = room_id;
  log_debug("next_unactive_room_index:%d", next_unactive_room_index);
}


//Client Logic
bool rooms_remove_from_zero(int socket_id){ //Removes a client from starting room
  Room* room_zero = rooms_get_room_by_id(0);
  log_debug("Removing client with socket_it %d from starting room", socket_id);
  return room_remove_client(room_zero, socket_id);
}

bool rooms_move_to_zero(Client* client, int old_room_id){ //Removes from current room and moves to room zero

  Room* old_room = rooms[old_room_id];
  if(old_room == NULL){ //unexpected behaviour
    log_error("Trying to access NULL room");
    return false;
  }

  if(client == NULL) { //unexpected behaviour
    log_error("Trying to move NULL client from room:%d", old_room);
    return false;
  }

  Room* room_zero = rooms_get_room_by_id(0);
  bool status = room_add_client(room_zero, client);
  if(status){
    status = room_remove_client(old_room, client->socket_id);
    log_debug("Moved client %s to starting room", client_to_string(client));
  } else{
    log_error("Couldn't add client %d to starting room, forcing disconnection...", client->socket_id);
    // socket_diconnect_client(client->socket_id); //TODO IN REALTA' NON LA VEDE STA FUNZIONE!!! Metterla nel main??
    rooms_remove_destroy_client(client);
  }
  return status;
}

void rooms_remove_destroy_client(Client* client) { //Removes client from room and desroy client

  if (client == NULL) { //unexpected behaviour
    log_error("Trying to delete NULL client");
    return;
  }

  if(rooms[client->room_id] == NULL){ //unexpected behaviour
    log_error("Trying to remove client from null room, we'll just destroy it...");
    client_destroy(client);
    return;
  }

  Room* room = rooms[client->room_id];
  
  if(room_remove_client(rooms[client->room_id], client->socket_id)) //remove client from room and
    client_destroy(client);                                         //destroy it
  else
    log_error("Oh no è successo un casino con le stanze, buona fortuna a debuggarlo =)"); //TODO
  
  if(room->id == 0) {//don't destroy starting room
    log_debug("Client removed and destroyed from room 0 (log da togliere)"); //TODO
    return;
  }

  if(room_is_empty(room)){ 
    log_debug("Room %d is empty, deleting it.", room->id);
    room_delete(room);
    return;
  }

}

void rooms_remove_from_room(int socket_id, int room_id) { //Auxiliar function: Removes client from room
  if((!is_valid_room_id(room_id)) || (rooms[room_id] == NULL)){ //unexpected behaviour
    log_error("Trying to delete client with socket_id %d from invalid room id %d", socket_id, room_id);
    return;
  }

  Client* c = room_get_client_by_id(rooms[room_id], socket_id);
  if(c != NULL){
    rooms_remove_destroy_client(c);
  } else { //unexpected behaviour
    log_error("Trying to delete client n.%d from room n.%d, but it is not there!", socket_id, room_id);
  }

}


//Prints and Debug
void print_rooms() { //Debug function
  for(int i = 0; i<MAX_CLIENTS; i++) {
    if(rooms[i] != NULL)
      room_print(rooms[i]);
    else
      printf("%d : NULL ROOM\n", i);
  }
}



