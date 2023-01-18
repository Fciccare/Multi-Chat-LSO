#!/bin/bash
gcc  server.c objects/user.c objects/client.c objects/room.c handler/socket_handler.c database/database.c database/library/sqlite3.c -o server -ldl -pthread && ./server
