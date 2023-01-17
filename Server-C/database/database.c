#include <stdio.h>
#include "library/sqlite3.h"
#include "database.h"
#include <string.h>

void errorHandler(char[]);

sqlite3* db;

int main(){
    sqlite3_open("database.db", &db);
    createTable();
    sqlite3_close(db);
    return 0;
} 

/* SQL query for creating tables */
void createTable(){
    char query[] =
        //"DROP TABLE IF EXISTS Users;"
        "CREATE TABLE IF NOT EXISTS Users("
        "username VARCHAR(25) PRIMARY KEY, "
        "password VARCHAR(20) NOT NULL, "
        "image BLOB);";

    if(sqlite3_exec(db, query, NULL, NULL, NULL) == SQLITE_OK){
        printf("Table created or alreary created\n");
    } else errorHandler("Error on table creation");
}

void errorHandler(char text[]){
    perror(text);
    sqlite3_close(db);
}

bool isExistingUser(){
    /*TODO*/
}