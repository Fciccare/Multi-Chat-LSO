#ifndef ROOMS_HANDLER_H
#define ROOMS_HANDLER_H

#include <stdbool.h>

#include "../objects/room.h"

#define MAX_ROOMS 64 //Max rooms that can exist + 1 (room 0 is th starting room)

bool add_room(Room* room);
void delete_room(unsigned int room_id);
int rooms_getActive_rooms();
void init_first_room();
Room* get_room_by_id(int room_id);
Client* get_user_by_id(int client_socket_id);
void get_formatted_room(int i, char* buff);

unsigned int find_next_unactive_room_index();
bool remove_from_zero(int socket_id);

void print_rooms(); //debug function

#endif