#ifndef SOCKET_HANDLER_H
#define SOCKET_HANDLER_H

#include <stdbool.h>

#include "../database/database.h"
#include "rooms_handler.h"

void socketDispatcher(int* client, char* buffer);

void broadcastMessageRoom(char* message, int* clients);

void login(char* message, int* client);
void registerUser(char* message, int* client);
bool log_user(User* u, int client_socket_id);

void createRoom(char* message, int* client_socket_id);

void getList(int* client_socket_id);

void request_to_enter_room(char* message, int* client_socket_id);
void not_accept_request(char* message);
void accept_request(char* message);

#endif