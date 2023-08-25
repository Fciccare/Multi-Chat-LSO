#ifndef USER_H
#define USER_H

//Structure//

typedef struct{
    char name[32];
    char password[256];
} User;

//Functions//

//Construtors and Destroy
User* user_create_default();
User* user_create(const char* name, const char* password);
void user_destroy(User* self);


//set
void user_setName(User* user, const char* name);
void user_setPassword(User* user, const char* password);

//Debug
char* user_to_string(User* user);

#endif