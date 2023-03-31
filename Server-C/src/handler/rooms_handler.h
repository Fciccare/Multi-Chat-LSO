#ifndef ROOMS_HANDLER_H
#define ROOMS_HANDLER_H

#include <stdbool.h>

#include "../objects/room.h"

#define MAX_ROOMS 64 //Max rooms that can exist + 1 (room 0 is th starting room)
extern unsigned int rooms_active;

//Init and Destroy
void init_starting_room();
//TODO: rooms_destroy();

//Get
Room* rooms_get_room_by_id(unsigned int room_id);
Client* rooms_get_client_from_room_by_id(int room_id, int client_socket_id);
Client* rooms_get_client_by_id(int client_socket_id);
void rooms_get_formatted_room(int i, char* buff);
bool is_valid_room_id(int id); //return (0 <= id < MAX_ROOMS);
bool rooms_is_empty(int room_id);

//Rooms Logic
void update_next_unactive_room_index();
bool rooms_add_room(Room* room);
void rooms_delete_room(unsigned int room_id);

//Client Logic
bool rooms_remove_from_zero(int socket_id);
bool rooms_move_to_zero(Client* client, int old_room_id);
void rooms_delete_client(Client* client);
void rooms_delete_client_from_room(int socket_id, int room_id);
void rooms_remove_from_and_destory(Room* r, Client* c);

//Prints and Debug
void print_rooms(); //debug function

#endif