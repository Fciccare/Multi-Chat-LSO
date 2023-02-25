#include <stdio.h>
#include "library/sqlite3.h"
#include "database.h"
#include <string.h>

#include "../library/log.h"

void errorHandler(char[]);

sqlite3* db;

// int main(){
//     sqlite3_open("database.db", &db);
//     createTable();
//     //insertUser("CELENTANI", "DEVASTATA");
//     isExistingUser("CELENTANI", "DEVASTATA");
//     sqlite3_close(db);
//     return 0;
// }

//TODO: capire come gestire gli errori

void traceCallback( void* udp, const char* sql ) { 
    log_debug("SQL: {%s}", sql); 
} 

void initDatabase(bool debug){
    log_debug("Starting database...");
    sqlite3_open("database/database.db", &db);
    if(debug)
        sqlite3_trace(db, traceCallback, NULL);
    createTable();
}

void createTable(){ //SQL query for creating tables
    char* error = 0;
    char query[] =
        //"DROP TABLE IF EXISTS Users;";
        "CREATE TABLE IF NOT EXISTS Users("
        "username VARCHAR(25) PRIMARY KEY, "
        "password VARCHAR(20) NOT NULL, "
        "image BLOB,"
        "online_status BOOL NOT NULL);"
        "UPDATE Users SET online_status = 0;";

    if(sqlite3_exec(db, query, NULL, NULL, &error) == SQLITE_OK){
        log_debug("Table created or alreary created");
    } else {
        errorHandler(error);
        sqlite3_free(error);
    }
}

void closeDatabase() { 
    sqlite3_close(db); 
}

bool insertUser(char username[], char password[]){
    char* error = 0;
    sqlite3_stmt* stmt;
    char query[] = "INSERT INTO Users (username, password, online_status) VALUES(?, ?, 0);";

    sqlite3_prepare_v2(db,query,strlen(query), &stmt, NULL);
    if (stmt == NULL) {
        errorHandler("Error Creation Statement");
        return false;
    }
    sqlite3_bind_text(stmt, 1, username, strlen(username), SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, password, strlen(password), SQLITE_TRANSIENT);

    if(sqlite3_step(stmt) == SQLITE_DONE){
        log_debug("Insert successful");
    } else {
        errorHandler("Insert failed");
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}


bool isExistingUser(char username[], char password[]) {
    sqlite3_stmt* stmt;
    char query[] =
        "SELECT * FROM Users WHERE username = ? and password = ?;";

    sqlite3_prepare_v2(db, query, strlen(query), &stmt, NULL);
    if (stmt == NULL) {
        errorHandler("Error Creation Statement");
        return false;
    }
    sqlite3_bind_text(stmt, 1, username, strlen(username), SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, password, strlen(password), SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        log_debug("User %s with pw %s exists", username, password);
    } else {
        errorHandler("User doesn't exists");
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool isLoggedExistingUser(char username[], char password[]) {
    sqlite3_stmt* stmt;
    char query[] = "SELECT * FROM Users WHERE username = ? and password = ? and online_status = 0;";
    sqlite3_prepare_v2(db, query, strlen(query), &stmt, NULL);
    if (stmt == NULL) {
        errorHandler("Error Creation Statement");
        return false;
    }
    sqlite3_bind_text(stmt, 1, username, strlen(username), SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, password, strlen(password), SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        log_debug("User %s with pw %s exists", username, password);
    } else {
        errorHandler("User doesn't exists or already logged");
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}


void dbUpdateStatus(char username[], char* status) {
    sqlite3_stmt* stmt;
        
    char query[] = "UPDATE Users SET online_status = ? WHERE username = ?;";
    sqlite3_prepare_v2(db, query, strlen(query), &stmt, NULL);
    if (stmt == NULL) {
        errorHandler("Error Creation Statement");
        return;
    }
    sqlite3_bind_text(stmt, 1, status, strlen(status), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, username, strlen(username), SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        log_debug("User logged in database");
    } else {
        errorHandler("User doesn't exists");
    }
    sqlite3_finalize(stmt);
}

void errorHandler(char text[]){
    log_error("%s", text);
    sqlite3_close(db);
}