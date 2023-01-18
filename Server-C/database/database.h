#ifndef DATABASE_H
#define DATABASE_H

#include <stdbool.h>

void createTable();
bool isExistingUser(char[], char[]);
bool insertUser(char[], char[]);
void initDatabase();
void closeDatabase();

#endif