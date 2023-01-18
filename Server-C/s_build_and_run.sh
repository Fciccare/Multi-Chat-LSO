#!/bin/bash
gcc -o server -pthread server.c struct/struct.c handler/socket_handler.c database/database.c database/library/sqlite3.c && ./server
