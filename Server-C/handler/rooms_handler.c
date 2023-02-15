#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "rooms_handler.h"

Room* rooms[MAX_ROOMS] = {0};
unsigned int rooms_active = 1; //room 0 is always active
unsigned int next_unactive_room_index = 0; //next empty spot to fill with new room
//0 must never be used as an id!

void init_first_room(){
  rooms[0] = room_create(0, "Defualt Room", NULL);
}

Room* get_room_by_id(int room_id){
  //TODO: Aggiungere controlli (forse)
  return rooms[room_id];
}

Client* get_user_by_id(int client_socket_id){
  return room_get_client_by_id(rooms[0], client_socket_id);
}

bool add_room(Room* new_room) {
  if (rooms_active == MAX_ROOMS) //if there is no space for a new room
    return false;
  
  next_unactive_room_index = find_next_unactive_room_index();
  // printf("Next_unactive_room_index: %d\n", next_unactive_room_index); //debug print
  rooms[next_unactive_room_index] = new_room;

  new_room->id=next_unactive_room_index;
  rooms_active++;
  
  return true;
}

void delete_room(unsigned int room_id) {
  if(room_id == 0 || rooms[room_id] == NULL) {
    perror("Try deleting a non-existing room");
    return;
  }

  room_destroy(rooms[room_id]);
  rooms[room_id] = NULL;
  rooms_active--;
}

int rooms_getActive_rooms() {
  return rooms_active;
}

unsigned int find_next_unactive_room_index() {
  if (next_unactive_room_index == 0)
    next_unactive_room_index++;
  unsigned int i = next_unactive_room_index;
  while (true) { 
    if (rooms[i] != NULL) {
      if(i++ == MAX_ROOMS) //if we get out of the array
        i=1;               //we keep searching from the start (0 excluded)
        
    } else 
      return i;
  }
}

void print_rooms() { //debug function
  for(int i = 0; i<MAX_CLIENTS; i++) {
    if(rooms[i] != NULL)
      room_print(rooms[i]);
    else
      printf("%d : NULL ROOM\n", i);
  }
}

void get_formatted_room(int i, char* buff) { //Get a specific room in a formatted manner
  if (rooms[i] == NULL)
    buff[0] = '\0';
  else {
    sprintf(buff, "%d<>%s<>%d\n",i, rooms[i]->name, rooms[i]->clients_counter); //id<>nomeStanza<>clientConnessi
  }
}

bool remove_from_zero(int socket_id){ //removes a client from starting room
    Room* room_zero = get_room_by_id(0);
    return room_remove_client(room_zero, socket_id);
}

