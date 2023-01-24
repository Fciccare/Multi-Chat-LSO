#ifndef SOCKET_HANDLER_H
#define SOCKET_HANDLER_H

#include <stdbool.h>

#include "../database/database.h"
#include "rooms_handler.h"

void socketDispatcher(int* client, char* buffer);
void broadcastMessageRoom(char* message, int* clients);
void login(char* message, int* client);
void registerUser(char* message, int* client);
bool logged_user(User* u, int client_socket_id);
void createRoom(char* message, int* client_socket_id);
void getList(int* client_socket_id);
#endif