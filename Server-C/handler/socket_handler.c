#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "socket_handler.h"
// #include "../objects/user.h"


void socketDispatcher(int* client_socket_id, char* buffer, int* clients) {
    char tag[5] = {0};
    strncpy(tag, buffer, 5);     // get the tag
    char* message = buffer + 5;  // point to buffer without tag

    if (strncmp(tag, "[MSG]", 5) == 0) {
        broadcastMessageRoom(&(*message), client_socket_id);
    } else if (strncmp(tag, "[LGN]", 5) == 0) {
        login(&(*message), client_socket_id);
    } else if (strncmp(tag, "[RGT]", 5) == 0) {
        registerUser(&(*message), client_socket_id);
    } else {
        write(*client_socket_id,"Please send data with this tag: \n[MSG] SEND MESSAGE IN BROADCAST\n[LGN] LOGIN WITH EMAIL AND PASSWORD\n",102);
        printf("Send instruction\n");
    }
}

void broadcastMessageRoom(char* message, int* client_socket_id) {
    for (int k = 0; k < 1024; ++k) {
        if (*(client_socket_id + k) != 0) {
            printf("[Send to client %d]\n", *(client_socket_id + k));
            write(*(client_socket_id + k), message, strlen(message));
        }
    }
}

void login(char* message, int* client_socket_id) {
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
        if(logged_user(user_create(username, password), *client_socket_id)){
            write(*client_socket_id, "Login successful\n", 18);  // Remember: Java recv need string end with EOF
            printf("Login successful\n");
        }else{
            write(*client_socket_id, "Room full\n", 11);  // Remember: Java recv need string end with EOF
            printf("Room full\n");
        }
    }else{
        write(*client_socket_id, "Login failed\n", 14);  // Remember: Java recv need string end with EOF
        printf("Login failed\n");
    }
}

bool logged_user(User* u, int client_socket_id){
    Client* client = client_create(u, client_socket_id, 0);
    Room* room_zero = get_room_by_id(0);
    bool status = room_add_client(room_zero, client);

    for(int i = 0; i < room_zero->clients_counter; ++i){
        if(room_zero->clients[i] != NULL)
            client_print(room_zero->clients[i]);
    }

    return status;
}

void registerUser(char* message, int* client_socket_id) {
    char* text = strdup(message);

    char* username = NULL;
    char* password = NULL;

    username = strtok(text, "<>");
    password = strtok(NULL, "<>");
    password[strlen(password) - 1] = '\0';  // remove \n from a string
    text = NULL;

    if (insertUser(username, password)) {
        write(*client_socket_id, "Register successful\n", 21);  // Remember: Java recv need string end with EOF
        printf("Register successful\n");
    } else {
        write(*client_socket_id, "Register failed\n", 17);  // Remember: Java recv need string end with EOF
        printf("Register failed\n");
    }
}