#ifndef SOCKET_HANDLER_H
#define SOCKET_HANDLER_H

#include <stdbool.h>

#include "../database/database.h"
#include "rooms_handler.h"

//Tags: [MSG], [LGN], [RGT], [CRT], [LST], [RQT], [ACC], [NAC], [EXT]

//Main Dispatcher
bool socketDispatcher(int* client, char* buffer);


//Request Processing
void broadcast_message_into_room(char* message, int* clients);

void login(char* message, int* client);
void register_user(char* message, int* client);

void create_room(char* message, int* client_socket_id);
void get_list(int* client_socket_id); 

void request_to_enter_room(char* message, int* client_socket_id);
void accept_request(char* message);
void not_accept_request(char* message);

bool exit_room(char* message, int* client_socket_id);

//TODO
// void socket_disconnect_client(int socket_id); //SPOSTARE IN MAIN??

//Auxiliar
bool notify_new_master(int room_id);
bool log_user(User* u, int client_socket_id);
#endif