#ifndef SOCKET_HANDLER_H
#define SOCKET_HANDLER_H

#include <stdbool.h>

#include "../database/database.h"
#include "rooms_handler.h"

bool socketDispatcher(int* client, char* buffer);

void broadcast_message_into_room(char* message, int* clients);

void login(char* message, int* client);
void register_user(char* message, int* client);
bool log_user(User* u, int client_socket_id);

void create_room(char* message, int* client_socket_id);

void get_list(int* client_socket_id);

void request_to_enter_room(char* message, int* client_socket_id);
void not_accept_request(char* message);
void accept_request(char* message);

bool exit_room(char* message, int* client_socket_id);

#endif