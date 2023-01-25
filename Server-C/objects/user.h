#ifndef USER_H
#define USER_H

//Structure//

typedef struct{
    char name[32];
    char password[256];
    char* propic;
} User;

//Functions//

//DISCLAIMER: Non ci volgio pensare per ora come gestire la propic (penso sia uguale ma poi vedo)
//Tutte le funzioni non gestiscono la propic, i costruttori hanno u->propic=0;

//Construtors and Destroy

User* user_create_default();
User* user_create(const char* name, const char* password);
void user_destroy(User* self);

//set
void user_setName(User* user, const char* name);
void user_setPassword(User* user, const char* password);


//Other functions
void user_print(User* user);

#endif