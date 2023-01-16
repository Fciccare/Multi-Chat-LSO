#ifndef SOCKET_HANDLER_H
#define SOCKET_HANDLER_H

void socketDispatcher(int* client, char* buffer, int* clients);
void broadcastMessageRoom(char* message, int* clients);

#endif