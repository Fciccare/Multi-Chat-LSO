#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "rooms_handler.h"
#include "../library/log.h"

#define MAX_ROOMS 64 //Max rooms that can exist + 1 (room 0 is th starting room)

Room* rooms[MAX_ROOMS] = {0};
unsigned int rooms_active = 1; //room 0 (starting room) is always active
unsigned int next_unactive_room_index = 1; //next empty spot to fill with new room(always updated!)
//NOTICE: 0 must never be used as an id!

pthread_mutex_t rooms_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t room_mutexes[MAX_ROOMS] = {0};

/*TODO: (VAI VALE U CAN DO IT (si me lo dico da sola))

FIXARE! 
Se [EXT] inviato per uscire dalla app -> seg fault su free del client
*/



//Init and Destroy
void rooms_init(){
  log_debug("Locking rooms_mutex before initialization of room 0 and room_mutexes");
  pthread_mutex_lock(&rooms_mutex);

  rooms[0] = room_create(0, "Defualt Room", NULL);
  for(int i = 0; i < MAX_ROOMS; i++){
    pthread_mutex_init(&room_mutexes[i], NULL); 
  }
  log_debug("Unlocking rooms_mutex after initization of room 0 and room_mutexes");
  pthread_mutex_unlock(&rooms_mutex);

  log_info("Rooms initalized: SERVER READY");
}

void rooms_destroy(){ //To be called when closing server
  for (int i = 0; i++; i<MAX_ROOMS){
    if(rooms[i] != NULL){
      room_clear(rooms[i]);
      room_delete(rooms[i]);
    }
    pthread_mutex_destroy(&room_mutexes[i]);
  }
  pthread_mutex_destroy(&rooms_mutex);
  rooms_active = 0;
  next_unactive_room_index = 1;
  log_debug("Rooms destroyed");
}


//Get
Room* rooms_get_room_by_id(unsigned int room_id){ //NEED TO BE CALLED WITHIN MUTEX LOCK

  if(room_id >= MAX_ROOMS){
    log_error("Error: rooms_get_room_by_id, out of bounds array index");
    return NULL;
  }

  return rooms[room_id];
}

Client* rooms_get_client_from_room_by_id(int room_id, int client_socket_id){ //NEED TO BE CALLED WITHIN MUTEX LOCK
  //NOTICE: this function must be called only within mutex lock
  return room_get_client_by_id(rooms[room_id], client_socket_id);
}

Client* rooms_get_client_by_id(int socket_id){ //HAS MUTEX LOCK INSIDE
  Client* c = NULL;

  log_debug("Locking rooms_mutex before searching for client %d", socket_id);
  pthread_mutex_lock(&rooms_mutex);
  int max = rooms_active;
  for (int i = 0; i < MAX_ROOMS; i++){
    if (rooms[i] != NULL) {
      if ((c = rooms_get_client_from_room_by_id(i, socket_id)) != NULL)
        break;
    }
    if (i == max) 
      break;
  }
  log_debug("Unlocking rooms_mutex after searching for client %d", socket_id);
  pthread_mutex_unlock(&rooms_mutex);
  return c;
}

void rooms_get_formatted_room(int i, char* buff) { //Get specific room in formatted manner HAS MUTEX LOCK INSIDE


  log_debug("Locking room_mutexes[%d] before getting formatted room %d", i, i);
  pthread_mutex_lock(&room_mutexes[i]);
  if (rooms[i] == NULL) //If room is not active, returns terminatoin character
    buff[0] = '\0';
  else {
    sprintf(buff, "%d<>%s<>%d\n", i, rooms[i]->name, rooms[i]->clients_counter); // roomID<>roomNAME<>clientsConnected
  }
  log_debug("Unlocking room_mmutexes[%d] after getting formatted room %d", i, i);
  pthread_mutex_unlock(&(room_mutexes[i]));
}

bool is_valid_room_id(int id) { //NEED TO BE CALLED WITHIN MUTEX LOCK
  //return (0 <= id < MAX_ROOMS);
  return (0<= id && id < MAX_ROOMS) ? true : false;
}


//Rooms Logic
void update_next_unactive_room_index() { //Auxiliar function to rooms_add_room NEEDS TO BE CALLED WITHIN MUTEX LOCK

  //NOTICE: this function is called only within mutex lock
  //        so we don't need to lock mutex here
  //        (we can't call this function outside mutex lock OR BAD THINGS MIGHT HAPPEN)

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
      // log_debug("next_unactive_room_index:%d", i);
      next_unactive_room_index = i;
      return;
    }

    if(next_unactive_room_index == i){//unexpected behaviour: we looked the entire array, didn't find NULL in any
      log_error("no NULL elements in rooms array");
      return;
    }
  }
}

bool rooms_add_room(Room* new_room) { //HAS MUTEX LOCK INSIDE

  if(new_room == NULL){ //unexpected behaviour
    log_error("Error: Trying to add null room to rooms array");
    return false;
  }

  log_debug("Locking rooms_mutex before adding a new room");
  pthread_mutex_lock(&rooms_mutex);

  if (rooms_active == MAX_ROOMS){ //if there is no space for a new room
    log_warn("Max Room reached, unable to create new room");
    pthread_mutex_unlock(&rooms_mutex);
    return false;
  }
  
  log_debug("Next unactive room index that will be used: %d", next_unactive_room_index);
  log_debug("Locking room_mutexes[%d] before adding it to array", next_unactive_room_index);
  pthread_mutex_lock(&room_mutexes[next_unactive_room_index]);

  rooms[next_unactive_room_index] = new_room;
  rooms_active++;
  new_room->id=next_unactive_room_index;
  new_room->master_client->room_id=next_unactive_room_index;

  log_debug("New room assegned to array, set new room id, set master client room id, increment rooms_active counter");
  log_debug("Unlocking room_mutexes[%d] after adding it to array", next_unactive_room_index);
  pthread_mutex_unlock(&room_mutexes[next_unactive_room_index]);

  if(rooms_active<MAX_ROOMS)
    update_next_unactive_room_index();
  
  log_debug("Unlocking rooms_mutex after adding a new room");
  pthread_mutex_unlock(&rooms_mutex);
  
  return true;
}
 
void rooms_delete_room(unsigned int room_id) {

  //NOTICE: this function is called only within mutex lock of room_mutexs[room_id] and locks rooms_mutex

  log_debug("Locking rooms_mutex before deleting room");
  pthread_mutex_lock(&rooms_mutex);

  Room* room = rooms[room_id];
  if(room_id == 0 || room_id >= MAX_ROOMS || room == NULL) { //unexpected behaviour
    log_error("Trying to delete non-existing room (room_id:%d)", room_id);
    pthread_mutex_unlock(&rooms_mutex);
    return;

  } else {
    log_debug("Room exists , removing clients and destroying ...");

    if(rooms[room_id] == NULL) {
      log_debug("Room was destroyed by another thread, exiting after unlocking room_mutexes[%d] and rooms_mutex", room_id);
      pthread_mutex_unlock(&rooms_mutex);
      return;
    } 
  }

  if(!(room_is_empty(room))){
    log_debug("Room is not empty, moving clients to room 0 (locking room_mutexes[0] before)");
    pthread_mutex_lock(&room_mutexes[0]);
    for (int i = 0; i < MAX_CLIENTS; i++)  {
        Client* client_to_move = room->clients[i];
        if(client_to_move != NULL) {
          rooms_move_to_zero(client_to_move, room_id); //TODO QUA METTE IL LOCK E NON VA BENE T-T
        }
      }
  }

  room_delete(rooms[room_id]);
  rooms[room_id] = NULL;
  rooms_active--;
  log_debug("Room deleted, number of active rooms:%d, unlocking room_mutexes[%d] after delete", rooms_active, room_id);
  pthread_mutex_unlock(&room_mutexes[room_id]);

  if(room_id<next_unactive_room_index)
    next_unactive_room_index = room_id;
  log_debug("next_unactive_room_index:%d", next_unactive_room_index);
  

  log_debug("Unlocking rooms_mutex after deleting room");
  pthread_mutex_unlock(&rooms_mutex);
}


//Client Logic
bool rooms_remove_from_zero(int socket_id){ //Removes a client from starting room
  log_debug("Locking room_mutexes[0] before removing client from starting room");
  pthread_mutex_lock(&room_mutexes[0]);

  Room* room_zero = rooms_get_room_by_id(0);
  log_debug("Removing client with socket_it %d from starting room", socket_id);
  bool status = room_remove_client(room_zero, socket_id);

  log_debug("Unlocking room_mutexes[0] after removing client from starting room");
  pthread_mutex_unlock(&room_mutexes[0]);

  return status;
}

bool rooms_remove_from(Client* client){
  //HAS MUTEX LOCK INSIDE
  if(client == NULL){ //unexpected behaviour
    log_error("Trying to remove NULL client from room");
    return false;
  }

  int room_id = client->room_id;
  log_debug("Locking room_mutexes[%d] before removing client", room_id);
  pthread_mutex_lock(&room_mutexes[room_id]);

  if (rooms[room_id] == NULL) {
    log_error("Trying to delete client with socket_id %d from NULL room n.%d", client->socket_id, room_id);
    pthread_mutex_unlock(&room_mutexes[room_id]);
    return false;
  }

  bool status = room_remove_client(rooms[room_id], client->socket_id);
  if(room_id != 0 && rooms[room_id]->clients_counter == 0){ //if room is empty and not starting room
    log_debug("Room is empty and deleted inside, deleting it from rooms array");
    rooms[room_id] = NULL;
    rooms_active--;
  }

  log_debug("Unlocking room_mutexes[%d] after removing client", room_id);
  pthread_mutex_unlock(&room_mutexes[room_id]);
  return status;
} 

// bool rooms_move_to_zero_wrapper(Client* client, int old_room_id){ //Called by socket_handler
//   log_debug("Locking room_mutexes[%d] before removing client from old room", old_room_id);
//   pthread_mutex_lock(&room_mutexes[old_room_id]);
//   bool status = rooms_move_to_zero(client, old_room_id);
//   log_debug("Unlocking room_mutexes[%d] after removing client from old room", old_room_id);
//   pthread_mutex_unlock(&room_mutexes[old_room_id]);
//   return status;
// }

bool rooms_move_to_zero(Client* client, int old_room_id){ //Removes from current room and moves to room zero 
  
  //NOTICE: this function is called only within mutex lock of room_mutexs[old_room_id] and locks room_mutexes[0]

  // Room* old_room = rooms[old_room_id];
  // if(old_room == NULL){ //unexpected behaviour
  //   log_error("rooms_move_to_zero: Trying to access NULL room");
  //   return false;
  // }

  // if(client == NULL) { //unexpected behaviour
  //   log_error("rooms_move_to_zero: Trying to move NULL client from room:%d", old_room);
  //   return false;
  // }

  bool status = rooms_remove_from(client); //This function has mutex lock inside

  log_debug("Locking room_mutexes[0] before adding client to starting room");
  pthread_mutex_lock(&room_mutexes[0]);

  Room* room_zero = rooms_get_room_by_id(0);
  status = room_add_client(room_zero, client) && status;
  if(status){
    log_debug("Moved client %s to starting room", client_to_string(client));

    //Every room is destroyed when empty, so we don't need to check if it's empty
    // if(room_is_empty(old_room)){
    //   log_debug("Room is empty, deleting room...");
    //   rooms_delete_room(old_room->id);
    // }
  } else {
    log_error("Couldn't move client %d to starting room, forcing disconnection...", client->socket_id);
    // socket_diconnect_client(client->socket_id); //TODO IN REALTA' NON LA VEDE STA FUNZIONE!!! Metterla nel main??
    client_destroy(client);
  }


  log_debug("Unlocking room_mutexes[0] after moving client to starting room", old_room_id);
  pthread_mutex_unlock(&room_mutexes[0]);
  return status;
}

void rooms_remove_destroy_client_wrapper(Client* client) { //Removes client from room (uses rooms_remove_destroy_client)
  
  //HAS MUTEX LOCK INSIDE

  if (client == NULL){
    log_error("client NULL passed to room_remove_destroy_client_wrapper");
    return;
  }

  int room_id = client->room_id;
  log_debug("Locking room_mutexes[%d] before removing client", room_id);
  pthread_mutex_lock(&room_mutexes[room_id]);

  if (rooms[room_id] == NULL) {
    log_error("Trying to delete client with socket_id %d from NULL room n.%d", client->socket_id, room_id);
    pthread_mutex_unlock(&room_mutexes[room_id]);
    return;
  }

  rooms_remove_destroy_client(client); //This function has mutex lock 


  log_debug("Unlocking room_mutexes[%d] after removing client", room_id);
  pthread_mutex_unlock(&room_mutexes[room_id]);

}

void rooms_remove_destroy_client(Client* client) { //Removes) client from room and desroy client

  //NOTICE: this function is not thread safe, it should be called only while holding the room_mutex of the room
  //It is called by rooms_remove_from_room (which has the mutex lock inside)
  //TESSSSSSSSSSSSTAAAAAAAAAAA

  if (client == NULL) { //unexpected behaviour
    log_error("Trying to delete NULL client");
    return;
  }

  Room* room = rooms[client->room_id];
  if(room == NULL){ //unexpected behaviour
    log_warn("Trying to remove client from null room, we'll just destroy it...");
    client_destroy(client);
    return;
  }
  
  if(room_remove_client(room, client->socket_id)) //remove client from room and
    client_destroy(client);                       //destroy it
  else
    log_error("Oh no è successo un casino con le stanze, buona fortuna a debuggarlo =)"); //TODO
  
  if(room->id == 0) {//don't destroy starting room
    log_debug("Client removed and destroyed from room 0 (log da togliere)"); //TODO
    return;
  }

// room_remove_client cancella la stanza se serve
  // if(room_is_empty(room)){ 
  //   log_debug("Room %d is empty, locking rooms_mutex and deleting room.", room->id);
  //   pthread_mutex_lock(&rooms_mutex);
  //   room_delete(room);
  //   rooms[client->room_id] = NULL;
  //   rooms_active--;
  //   log_debug("rooms_mutex after deleting room", client->room_id);
  //   pthread_mutex_unlock(&rooms_mutex);
  // }

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



