#ifndef DATABASE_H
#define DATABASE_H

#include <stdbool.h>

void initDatabase(bool);
void createTable();
void closeDatabase();

bool insertUser(char[], char[]);

bool isExistingUser(char[], char[]);
bool isLoggedExistingUser(char[], char[]);

void dbUpdateStatus(char[], char*);

#endif