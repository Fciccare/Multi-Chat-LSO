#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "socket_handler.h"
#include "../database/database.h"

void socketDispatcher(int* client, char* buffer, int* clients) {
    char tag[5] = {0};
    strncpy(tag, buffer, 5);     // get the tag
    char* message = buffer + 5;  // point to buffer without tag

    if (strncmp(tag, "[MSG]", 5) == 0) {
        broadcastMessageRoom(&(*message), clients);
    } else if (strncmp(tag, "[LGN]", 5) == 0) {
        login(&(*message), client);
    } else if (strncmp(tag, "[RGT]", 5) == 0) {
        registerUser(&(*message), client);
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

void login(char* message, int* client) {
    // write(*client, "Login successful\n", 18);  // Remember: Java recv need string end with EOF
    // printf("Send Login successful\n");
    char* text = strdup(message);

    char* username = NULL;
    char* password = NULL;

    username = strtok(text, "<>");
    password = strtok(NULL, "<>");
    password[strlen(password)-1] = '\0';  // remove \n from a string
    text = NULL;

    if(isExistingUser(username, password)){
        write(*client, "Login successful\n", 18);  // Remember: Java recv need string end with EOF
        printf("Login successful\n");
    }else{
        write(*client, "Login failed\n", 14);  // Remember: Java recv need string end with EOF
        printf("Login failed\n");
    }
}

void registerUser(char* message, int* client) {
    char* text = strdup(message);

    char* username = NULL;
    char* password = NULL;

    username = strtok(text, "<>");
    password = strtok(NULL, "<>");
    password[strlen(password) - 1] = '\0';  // remove \n from a string
    text = NULL;

    if (insertUser(username, password)) {
        write(*client, "Register successful\n", 21);  // Remember: Java recv need string end with EOF
        printf("Register successful\n");
    } else {
        write(*client, "Register failed\n", 17);  // Remember: Java recv need string end with EOF
        printf("Register failed\n");
    }
}