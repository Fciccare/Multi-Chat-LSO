#ifndef DATABASE_H
#define DATABASE_H

#include <stdbool.h>

void createTable();
bool isExistingUser(char[], char[]);
bool isLoggedExistingUser(char[], char[]);
void dbUpdateStatus(char[], char*);
bool insertUser(char[], char[]);
void initDatabase(bool);
void closeDatabase();

#endif