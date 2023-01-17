#ifndef DATABASE_H
#define DATABASE_H

#include <stdbool.h>

void createTable();
bool isExistingUser(char[], char[]);
void insertUser(char[], char[]);

#endif