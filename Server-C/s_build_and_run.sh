#!/bin/bash
gcc -o server -pthread server.c objects/user.c objects/client.c objects/room.c handler/socket_handler.c && ./server
