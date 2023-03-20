#!/bin/bash
# TODO: Passare a make o ancora meglio cmake
gcc -o server \
    objects/user.c objects/client.c objects/room.c \
    handler/socket_handler.c handler/rooms_handler.c \
    database/database.c database/library/sqlite3.c \
    library/log.c \
    server.c \
    -Wall -Wextra \
    -ldl -pthread -DLOG_USE_COLOR \
    && ./server -d
