#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

#include "user.h"

//Constructors and Destory
User* user_create_default() {
  User *u = (User*)malloc(sizeof(User));
  strcpy(u->name, "defaultName");
  strcpy(u->password, "change your password!");
  u->propic=0; //no propic? non so il DB come gestisce, contorllare
  return u;
}

User* user_create(const char* name, const char* password) {
  User *u = (User*)malloc(sizeof(User));
  strcpy(u->name, name);
  strcpy(u->password, password);
  u->propic=0; //no propic? non so il DB come gestisce, contorllare
  return u;
}

void user_destroy(User* u){
  free(u);
}


//set
void user_setName(User* u, const char* name) {
  strcpy(u->name, name);
}

void user_setPassword(User* u, const char* password) {
  strcpy(u->password, password);
}

//Other functions
//Si deve pensare alle altre funzionalità di User

void user_print(User* u) {
  printf("USER: Nome: %s, Password: %s\n", u->name, u->password);
}