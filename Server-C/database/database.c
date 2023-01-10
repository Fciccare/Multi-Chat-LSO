#include <stdio.h>
#include "sqlite3.h"

int main(){
    sqlite3* db;
    sqlite3_open("database.db", &db);
    
    return 0;
}