#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

#include "user.h"
#include "../library/log.h"


//Constructors and Destory
User* user_create_default() {
  User *u = (User*)malloc(sizeof(User));
  strcpy(u->name, "defaultName");
  strcpy(u->password, "change your password!");
  log_debug("Created: Default %s", user_to_string(u));
  return u;
}

User* user_create(const char* name, const char* password) {
  User *u = (User*)malloc(sizeof(User));
  strcpy(u->name, name);
  strcpy(u->password, password);
  log_debug("Created: %s", user_to_string(u));
  return u;
}

void user_destroy(User* u){
  if(u == NULL){
    log_error("Trying to destroy NULL user");
    return;
  }
  log_debug("destroying user: %s", user_to_string(u));
  free(u);
  u = NULL;
}


//Set
void user_setName(User* u, const char* name) {
  strcpy(u->name, name);
}

void user_setPassword(User* u, const char* password) {
  strcpy(u->password, password);
}


//Debug

char* user_to_string(User* u) {
  char value[512];
  if(u != NULL){
    sprintf(value, "User: {Nome: %s, Password: %s}", u->name, u->password);
    return strdup(value);
  }
  return "User: NULL";
}