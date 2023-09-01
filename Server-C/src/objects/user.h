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

//Debug
char* user_to_string(User* user);

#endif