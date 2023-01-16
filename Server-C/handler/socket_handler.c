#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "socket_handler.h"

void socketDispatcher(int* client, char* buffer, int* clients) {
    char tag[5] = {0};
    strncpy(tag, buffer, 5);     // get the tag
    char* message = buffer + 5;  // point to buffer without tag

    if (strncmp(tag, "[MSG]", 5) == 0) {
        broadcastMessageRoom(&(*message), clients);
    } else if (strncmp(tag, "[LGN]", 5) == 0) {
        // funzione di login
        write(*client, "Login successful\n", 18);  // Rember: Java recv need string end with EOF
        printf("Send Login successful\n");
    } else if (strncmp(tag, "[RGT]", 5) == 0) {
        // funzione di registazione
        write(*client, "Register successful\n", 21);  // Rember: Java recv need string end with EOF
        printf("Send Register successful\n");
    } else {
        write(*client,"Please send data with this tag: \n[MSG] SEND MESSAGE IN BROADCAST\n[LGN] LOGIN WITH EMAIL AND PASSWORD\n",102);
        printf("Send instruction\n");
    }
}

void broadcastMessageRoom(char* message, int* clients){
    for (int k = 0; k < 1024; ++k) {
        if (*(clients+k) != 0) {
            printf("[Send to client %d]\n", *(clients + k));
            write(*(clients + k), message, strlen(message));
        }
    }
}