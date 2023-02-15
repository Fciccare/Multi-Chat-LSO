#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "user.h"
#include "client.h"

//Constructor and Destroy

Client* client_create(User* user, int socket_id, unsigned int room_id) {
  Client* c = (Client*)malloc(sizeof(Client));
  c->user = user;
  c->socket_id = socket_id;
  c->room_id = room_id;
  return c;
}

void client_destory(Client* c) {
  free(c);
}

//Set

void client_setUser(Client* c, User* user) {
  c->user= user;
}

void client_setSocket_id(Client* c, int socket_id) {
  c->socket_id = socket_id;
}

void client_setRoom_id(Client* c, unsigned int room_id) {
  c->room_id = room_id;
}

//Other Functions

void client_print(Client* c) { //Debug Funcion
  printf("CLIENT:\n");
  user_print(c->user);
  printf("Socket_id: %d\nRoom_id: %d\n", c->socket_id, c->room_id);
}  