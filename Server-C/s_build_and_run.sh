#!/bin/bash
gcc -o server -pthread server.c struct/struct.c handler/socket_handler.c && ./server
