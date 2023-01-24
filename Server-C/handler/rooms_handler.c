#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "rooms_handler.h"

Room* rooms[MAX_ROOMS] = {0};
unsigned int rooms_active = 1; //c'è la stanza 0 "sempre attiva"
unsigned int next_room_index = 0; //0 never used as an id 

void init_first_room(){
  rooms[0] = room_create(0, "Defualt Room", NULL);
}

Room* get_room_by_id(int room_id){
  //TODO: Aggiungere controlli (forse)
  return rooms[room_id];
}

Client* get_user_by_id(int client_socket_id){ // Please fix it ❤
  Room* room_zero = rooms[0];
  // room_print(room_zero);
  for(int i = 0; i < room_zero->clients_counter; ++i){
    if(room_zero->clients[i] != NULL && room_zero->clients[i]->socket_id == client_socket_id){
      return room_zero->clients[i];
    }
  }
  return NULL;
}

bool add_room(Room* new_room) {
  if (rooms_active == MAX_ROOMS)
    return false;
  
  next_room_index = find_next_room_index();
  // printf("Next_room_index: %d\n", next_room_index);
  rooms[next_room_index] = new_room;

  new_room->id=next_room_index;
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

unsigned int find_next_room_index() {
  if (next_room_index == 0)
    next_room_index++;
  unsigned int i = next_room_index;
  while (true) {
    if (rooms[i] != NULL) {
      if(i++ == MAX_ROOMS-1)
        i=1;
    } else return i;
  }
}

void print_rooms() {
  for(int i = 0; i<MAX_CLIENTS; i++) {
    if(rooms[i] != NULL)
      room_print(rooms[i]);
    else
      printf("%d : NULL ROOM\n", i);
  }
}

void get_formatted_room(int i, char* buff) {
  if (rooms[i] == NULL)
    buff[0] = '\0';
  else {
    printf("%s\n", rooms[i]->name);
    printf("%s<>%d\n",rooms[i]->name, rooms[i]->clients_counter);
    sprintf(buff, "%s<>%d\n",rooms[i]->name, rooms[i]->clients_counter);

  }
}

