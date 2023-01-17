#include <stdio.h>
#include "library/sqlite3.h"
#include "database.h"
#include <string.h>
#include <stdbool.h>

int main(){
    sqlite3* db;
    sqlite3_open("database.db", &db);
    
    return 0;
} 

/* SQL query for creating tables */
void createTable(){
    char query[] =    "CREATE TABLE IF NOT EXIST Users("
                      "username VARCHAR(20) PRIMARY KEY, "
                      "password VARCHAR(20) NOT NULL,"
                      "image BLOB ;)"; /*da vedere*/
}

bool isExistingUser(){
    /*TODO*/
}