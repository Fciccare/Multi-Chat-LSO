#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "user.h"
#include "client.h"
#include "../library/log.h"

//Constructor and Destroy
Client* client_create(User* user, int socket_id, unsigned int room_id) {
  Client* c = (Client*)malloc(sizeof(Client));
  c->user = user;
  c->socket_id = socket_id;
  c->room_id = room_id;
  log_debug("Created: %s", client_to_string_full(c));
  return c;
}

void client_destroy(Client* c) {
  if (c == NULL) {
    log_error("Trying to destroy NULL client");
    return;
  }
  log_debug("Destroying client with id: %d", c->socket_id);
  user_destroy(c->user);
  free(c);
  c = NULL;
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


//Debug
char* client_to_string_full(Client* c) {
  char value[512];
  if(c != NULL) {
    sprintf(value, "Client: {Socket_id: %d, Room_id: %d, %s}", c->socket_id, c->room_id, user_to_string(c->user));
    return strdup(value);
  }
  return "Client: NULL";
}

char* client_to_string(Client* c) {
  char value[512];
  if(c != NULL){
    sprintf(value, "Client: {Socket_id: %d\tRoom_id: %d}", c->socket_id, c->room_id);
    return strdup(value);
  } 
  return "Client: NULL";
}