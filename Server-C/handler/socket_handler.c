#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include "socket_handler.h"
#include "rooms_handler.h"
// #include "../objects/user.h"


void socketDispatcher(int* client_socket_id, char* buffer) {
    char tag[5] = {0};
    strncpy(tag, buffer, 5);     // get the tag
    char* message = buffer + 5;  // point to buffer without tag
    message = strdup(message);

    if (strncmp(tag, "[MSG]", 5) == 0) { //Message in broadcast room
        broadcastMessageRoom(&(*message), client_socket_id);
    } else if (strncmp(tag, "[LGN]", 5) == 0) { //Login user
        login(&(*message), client_socket_id);
    } else if (strncmp(tag, "[RGT]", 5) == 0) { //Register user 
        registerUser(&(*message), client_socket_id);
    } else if (strncmp(tag, "[CRT]", 5) == 0){ //Create room
        createRoom(&(*message), client_socket_id);
        print_rooms();//debug
    } else if (strncmp(tag, "[LST]", 5) == 0){ //List of room
        getList(client_socket_id);
    } else if (strncmp(tag, "[RQT]", 5) == 0) { //Request from client to enter in a room
        request_to_enter_room(&(*message), client_socket_id);
    } else if (strncmp(tag, "[ACC]", 5) == 0) { //Accept user in a room
        accept_request(&(*message));
    } else if (strncmp(tag, "[NAC]", 5) == 0) { //Denied access in a room
        not_accept_request(&(*message));
    } else {
        write(*client_socket_id,"Please send data with this tag: \n[MSG] SEND MESSAGE IN BROADCAST\n[LGN] LOGIN WITH EMAIL AND PASSWORD\n",102);
        printf("Send instruction\n");
    }
}

void accept_request(char* message){ //Rivedi i casi limiti strani (in inglese)
    //Retrdfgdfognofdgaival && Cast
    char* string_socket_id_client = strtok(message, "<>");
    char* string_room_id = strtok(NULL, "<>");

    int socket_id_client = atoi(string_socket_id_client);
    int room_id = atoi(string_room_id);

    //Insert into array
    Room* room = get_room_by_id(room_id);
    Client* client = get_user_by_id(socket_id_client);
    room_add_client(room, client);
    remove_from_zero(client->socket_id); //Returna un boolean 

    room_print(room); //Current room
    room_print(get_room_by_id(0));  // Room Zero

    //Send to Client information
    // printf("Socket id client: %d \nRoom id: %d\n", socket_id_client, room_id);//debug
   
    char text[25];
    sprintf(text, "Access accept<>%d\n", room->id);
    printf("Server is sending: '%s'(%ld)", text,strlen(text)); //debug
    write(socket_id_client, text, strlen(text));
}

bool remove_from_zero(int socket_id){
    Room* room_zero = get_room_by_id(0);
    return room_remove_client(room_zero, socket_id);
}

void not_accept_request(char* message){
    unsigned int client_socket_id = atoi(message);  // thx atoi
    char text[] = "Access denied\n";
    printf("Server is sending: '%s'(%ld)", text,strlen(text)); //debug
    write(client_socket_id, text, strlen(text));
}



void request_to_enter_room(char* message, int* client_socket_id){ //Send to master client Room request to join another client, please Valentina fix it 💔
    // client_socket_id is client requesting (no master client)
    unsigned int room_id = atoi(message);  // thx atoi
    printf("Room id chosen from client is: %d\n", room_id);//debug
    Room* room = get_room_by_id(room_id);
    Client* client = get_user_by_id(*client_socket_id);
    
    int master_client_socket_id = 0;
    if(room->master_client != NULL) //useless
        master_client_socket_id = room->master_client->socket_id;
    char buffer[50];
    sprintf(buffer, "[RQT]%d<>%s<>%d\n", *client_socket_id, client->user->name, room_id);
    printf("Server is sending: '%s'(%ld)", buffer,strlen(buffer)); //debug
    write(master_client_socket_id, buffer, strlen(buffer));
}

void createRoom(char* message, int* client_socket_id){
    Client* client = get_user_by_id(*client_socket_id);
    Room* room = NULL;

    if(*(message+strlen(message)-1) == '\n'){ //Java mette uno \n alla fine delle stringhe, lo togliamo
        *(message+strlen(message)-1) = '\0'; //Si ringrazia Francesco ❤
    }
    if(client != NULL)

    room = room_create(0, message, client);//crash   
    if(add_room(room)){
        char text[35];
        sprintf(text, "Room create successful<>%d\n", room->id);
        printf("Server is sending: '%s'(%ld)", text,strlen(text)); //debug
        write(*client_socket_id, text, strlen(text));  // Remember: Java recv need string end with EOF
        printf("Room created successful\n");
        remove_from_zero(client->socket_id);  // Returna un boolean
        return;
    }
    write(*client_socket_id, "Room create failed\n", 20);  // Remember: Java recv need string end with EOF
    printf("Room create failed\n");
}

void broadcastMessageRoom(char* message, int* client_socket_id) { //Send message to every client in room
    // for (int k = 0; k < 1024; ++k) {
    //     if (*(client_socket_id + k) != 0) {
    //         printf("[Send to client %d]\n", *(client_socket_id + k));
    //         write(*(client_socket_id + k), message, strlen(message));
    //     }
    // }
    // Client* client = get_user_by_id(socket_id_client);
    // "[MSG]Ciao bella fra<>5"
    char* message_to_send = strtok(message, "<>");
    char* string_room_id = strtok(NULL, "<>");

    int room_id = atoi(string_room_id);
    int length = strlen(message_to_send)+1;
    char text[length+15];

    Room* room = get_room_by_id(room_id);
    Client** clients = room->clients;
    int online_client = room->clients_counter;

    int count = 0;
    for(int i = 0; i < MAX_CLIENTS; ++i){
        if(count == online_client){
            return;
        }else{
            if((clients+i) != NULL){
                int client_id = (*(clients+i))->socket_id; //client_id changes at every loop, it is the destination socket id
                sprintf(text, "[MSG]%s<>%d\n", message_to_send, *client_socket_id); //[MSG]HelloWorld<>5  "5" is sender socket id
                printf("Server is sending: '%s'(%ld)", text,strlen(text)); //debug
                write(client_id, text, strlen(text));
                count++;
            }
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
            char buffer[20] = {0};
            strcpy(buffer, "Login successful\n");
            printf("Server is sending: '%s'(%ld)", buffer,strlen(buffer)); //debug
            write(*client_socket_id, buffer, strlen(buffer));  // Remember: Java recv need string end with EOF
            printf("Login successful\n");
        }else{
            write(*client_socket_id, "Room full\n", 11);  // Remember: Java recv need string end with EOF
            printf("Room full\n");
        }
    }else{
        char buffer[20] = {0};
        strcpy(buffer, "Login failed\n");
        printf("Server is sending: '%s'(%ld)", buffer,strlen(buffer)); //debug
        write(*client_socket_id, buffer, strlen(buffer));  // Remember: Java recv need string end with EOF
        printf("Login failed\n");
    }
}

bool logged_user(User* u, int client_socket_id){
    Client* client = client_create(u, client_socket_id, 0);
    Room* room_zero = get_room_by_id(0);
    bool status = room_add_client(room_zero, client);

    // for(int i = 0; i < room_zero->clients_counter; ++i){
    //     if(room_zero->clients[i] != NULL)
    //         client_print(room_zero->clients[i]);
    // }

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
        char buffer[25] = {0};
        strcpy(buffer, "Register successful\n");
        printf("Server is sending: '%s'(%ld)", buffer,strlen(buffer)); //debug
        write(*client_socket_id, buffer, strlen(buffer));  // Remember: Java recv need string end with EOF
        printf("Register successful\n");
    } else {
        char buffer[25] = {0};
        strcpy(buffer, "Register failed\n");
        printf("Server is sending: '%s'(%ld)", buffer,strlen(buffer)); //debug
        write(*client_socket_id, buffer, strlen(buffer));  // Remember: Java recv need string end with EOF
        printf("Register failed\n");
    }
}

void getList(int* client_socket_id) {
    char start[10];
    char end[] = "[/LST]\n";
    char buff[42] = {0}; //formatted room buffer

    sprintf(start, "[LST]%d\n", MAX_CLIENTS);
    printf("Server is sending: '%s'(%ld)", start,strlen(start)); //debug
    write(*client_socket_id, start, strlen(start));

    int tmpFound= 0; //quante stanze attive abbiamo trovato
    int i = 1; //indice per visitare tutto l'array
    while (tmpFound<(rooms_getActive_rooms()-1)) { //Se ha trovato tutte le stanze attive non continua a scorrere l'array delle stanze
        if(i>MAX_ROOMS) { //controllo per evitare seg fault
            perror("Qualcosa non va con l'array delle stanze");
        }
        get_formatted_room(i, buff); //ottiene nomeStanza<>clientConnessi, carattere di terminazione se non esiste
        if (buff[0] != '\0') {
            printf("Server is sending: '%s'(%ld)", buff,strlen(buff)); //debug
            write(*client_socket_id, buff, strlen(buff));
            bzero(buff, 42);
            tmpFound++;
        }
        i++;
    }
    printf("Server is sending: '%s'(%ld)", end,strlen(end)); //debug
    write(*client_socket_id, end, strlen(end));
    printf("Write room list successful");
}