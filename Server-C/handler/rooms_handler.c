#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "rooms_handler.h"

unsigned int rooms_active = 1; //c'è la stanza 0 "sempre attiva"
unsigned int next_room_index = 0; //0 never used as an id 

bool add_room(Room* new_room) {
  if (rooms_active == MAX_ROOMS)
    return false;
  
  next_room_index = find_next_room_index();
  rooms[next_room_index] = new_room;

  new_room->id=next_room_index;
  rooms_active++;
  
  return true;
}

void delete_room(unsigned int room_id) {
  
  if(rooms[room_id] == NULL) {
    perror("Try deleting a non-existing room");
    return;
  }

  room_destroy(rooms[room_id]);
  rooms[room_id] = NULL;
  rooms_active--;
}

unsigned int find_next_room_index() {
  if (next_room_index == 0)
    next_room_index++;
  unsigned int i = next_room_index;
  while (true) {
    if (rooms[i] == NULL) {
      if(i++ == MAX_ROOMS)
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

