#ifndef ROOMS_HANDLER_H
#define ROOMS_HANDLER_H

#include <stdbool.h>

#include "../objects/room.h"

#define MAX_ROOMS 64 //Max rooms that can exist + 1 (room 0 is th starting room)

void init_starting_room();

//getters
Room* rooms_get_room_by_id(unsigned int room_id);
Client* rooms_get_client_by_id(int room_id, int client_socket_id);
void rooms_get_formatted_room(int i, char* buff);
int rooms_getActive_rooms();
bool is_valid_room_id(int id); //return (0 <= id < MAX_ROOMS);

//logic
void update_next_unactive_room_index();
bool rooms_add_room(Room* room);
void delete_room(unsigned int room_id);
bool rooms_remove_from_zero(int socket_id);
bool rooms_move_to_zero(Client* client, int old_room_id);

//prints and debug
void print_rooms(); //debug function

#endif