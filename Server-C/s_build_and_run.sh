#!/bin/bash
gcc -o server \
    objects/user.c objects/client.c objects/room.c \
    handler/socket_handler.c handler/rooms_handler.c \
    database/database.c database/library/sqlite3.c \
    server.c \
    -ldl -pthread \
    && ./server
