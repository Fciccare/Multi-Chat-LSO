#include <stdio.h>
#include "sqlite3.h"
#include <string.h>
#include <stdbool.h>

int createTable();
bool isExistingUser();

int main(){
    sqlite3* db;
    sqlite3_open("database.db", &db);
    
    return 0;
} 

/* SQL query for creating tables */
int createTable(){
    char query[] =    "CREATE TABLE IF NOT EXIST Users("
                      "username VARCHAR(20) PRIMARY KEY, "
                      "password VARCHAR(20) NOT NULL,"
                      "image BLOB ;)"; /*da vedere*/

        return 0;
}

bool isExistingUser(){
    /*TODO*/
}