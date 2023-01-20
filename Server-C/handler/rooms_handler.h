#ifndef ROOMS_HANDLER_H
#define ROOMS_HANDLER_H

#include "../objects/room.h"
// #include "../objects/client.h"
#include <stdbool.h>

#define MAX_ROOMS 64 //Limite di stanze esistenti+1

bool add_room(Room* room);
void delete_room(unsigned int room_id);
void init_first_room();
Room* get_room_by_id(int room_id);
Client* get_user_by_id(int client_socket_id);

unsigned int find_next_room_index();

void print_rooms();

#endif