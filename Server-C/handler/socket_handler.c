#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "rooms_handler.h"
#include "socket_handler.h"
#include "../library/log.h"

void socketDispatcher(int *client_socket_id, char *buffer) {
  char tag[5] = {0};
  strncpy(tag, buffer, 5);    // get the tag
  char *message = buffer + 5; // point to buffer without tag
  message = strdup(message);  // get rid of tag

  if (strncmp(tag, "[MSG]", 5) == 0) { // Message sent in room
    broadcastMessageRoom(&(*message), client_socket_id);
  } else if (strncmp(tag, "[LGN]", 5) == 0) { // Login user
    login(&(*message), client_socket_id);
  } else if (strncmp(tag, "[RGT]", 5) == 0) { // Register user
    registerUser(&(*message), client_socket_id);
  } else if (strncmp(tag, "[CRT]", 5) == 0) { // Create room
    log_info("Create room actived");
    createRoom(&(*message), client_socket_id);
    print_rooms();                            // debug
  } else if (strncmp(tag, "[LST]", 5) == 0) { // Get List of room
    getList(client_socket_id);
  } else if (strncmp(tag, "[RQT]", 5) == 0) { // Request from client to enter in a room
    request_to_enter_room(&(*message), client_socket_id);
  } else if (strncmp(tag, "[ACC]", 5) == 0) { // Accept user in a room
    accept_request(&(*message));
  } else if (strncmp(tag, "[NAC]", 5) == 0) { // Denied access in a room
    not_accept_request(&(*message));
  } else {
    // TODO: Implemeti managing of errors
    write(*client_socket_id, "Please send data with this tag: \n[MSG] SEND MESSAGE IN BROADCAST\n[LGN] LOGIN WITH EMAIL AND PASSWORD\n", 102);
    printf("Send instruction\n");
  }
}

void accept_request(char *message) { // Accept user in a room
  // TODO: Rivedi i casi limiti strani

  char *string_socket_id_client = strtok(message, "<>");
  char *string_room_id = strtok(NULL, "<>");

  // DOMANDA: questo socket id è il master client o il client che è stato accettato? controllare, io su client java non capisco niente )=

  int socket_id_client = atoi(string_socket_id_client);
  int room_id = atoi(string_room_id);

  // Insert client into room
  Room *room = get_room_by_id(room_id);
  Client *client = get_user_by_id(socket_id_client);
  room_add_client(room, client);
  remove_from_zero(client->socket_id); // TODO: questo ritorna un boolean, vedere se lo vogliamo gestire

  // Debug prints: current and starting room
  room_print(room);
  room_print(get_room_by_id(0));

  // printf("Socket id client: %d \nRoom id: %d\n", socket_id_client, room_id);//Debug print

  // Send to Client it has been accepted
  char text[25];
  sprintf(text, "Access accept<>%d\n", room->id);
  printf("Server is sending: '%s'(%ld)", text, strlen(text)); // Debug print
  write(socket_id_client, text, strlen(text));
}

void not_accept_request(char *message) { // Don't accept a Client in a room
  // Send to Client it has not been accepted
  unsigned int client_socket_id = atoi(message);
  char text[] = "Access denied\n";
  printf("Server is sending: '%s'(%ld)", text, strlen(text)); // Debug print
  write(client_socket_id, text, strlen(text));
}

void request_to_enter_room(char *message, int *client_socket_id) {
  // TODO: CHE SE DEVE FIXAAA OOOOOOOOOOOO vabbè questa la metto a posto poi
  // Send to master client Room request to join another client, please Valentina fix it 💔(non so che devo fixà)

  // client_socket_id is client requesting to enter room
  unsigned int room_id = atoi(message);
  printf("Room id chosen from client is: %d\n", room_id); // Debug print
  Room *room = get_room_by_id(room_id);
  Client *client = get_user_by_id(*client_socket_id);

  int master_client_socket_id = 0;

  // if(room->master_client != NULL) //allo sto coso cerve a evitare il seg fault,
  // dovremmo valutare quanto vogliamo fare controlli
  // in teoira queste cose non dovrebbero succedere, ma di solito comunque si controlla

  master_client_socket_id = room->master_client->socket_id;
  char buffer[50];
  sprintf(buffer, "[RQT]%d<>%s<>%d\n", *client_socket_id, client->user->name, room_id);
  //[RQT]client_requesting_to_enter_socket_id<>user_name<>room_id
  printf("Server is sending: '%s'(%ld)", buffer, strlen(buffer)); // Debug print
  write(master_client_socket_id, buffer, strlen(buffer));
}

void createRoom(char *message, int *client_socket_id) {
  Client *client = get_user_by_id(*client_socket_id);
  log_info("Getted user id: %d", *client_socket_id);
  Room *room = NULL;

  if (*(message + strlen(message) - 1) == '\n') { // Remove '\n' from end of string that java puts
    *(message + strlen(message) - 1) = '\0';
    log_info("Removed newline from message");
  }
  
  if (client != NULL){
    room = room_create(0, message, client); // crash   ???
    log_info("Room created");
  } 
  
  if (add_room(room)) {
    char text[35];
    sprintf(text, "Room create successful<>%d\n", room->id);
    printf("Server is sending: '%s'(%ld)", text, strlen(text)); // Debug print
    write(*client_socket_id, text, strlen(text));               // Remember: Java recv need string end with EOF
    printf("Room created successful\n");
    remove_from_zero(client->socket_id); // TODO: Ritorna un boolean, forse lo vogliamo gestire?
    return;
  }

  // else: failed to create room
  write(*client_socket_id, "Room create failed\n", 20); // Java recv need string end with EOF
  printf("Room create failed\n");
}

void broadcastMessageRoom(char *message, int *client_socket_id) {
  // Send message to every client in room

  char *message_to_send = strtok(message, "<>");
  char *string_room_id = strtok(NULL, "<>");

  int room_id = atoi(string_room_id);

  int length = strlen(message_to_send) + 1;
  char text[length + 15];

  Room *room = get_room_by_id(room_id);
  Client **clients = room->clients;
  int online_client = room->clients_counter;

  int count = 0; // how many clients has been sent to
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (count == online_client) { // if message has been sent to every online client
      return;
    } else {
      if ((clients + i) != NULL) {
        int client_id = (*(clients + i))->socket_id; // client_id changes at every loop, it's the destination socket id
        
        sprintf(text, "[MSG]%s<>%d\n", message_to_send, *client_socket_id);
        //[MSG]message_to_send<>sender_socket_id
        printf("Server is sending: '%s'(%ld)", text, strlen(text)); // Debug print
        write(client_id, text, strlen(text));

        count++;
      }
    }
  }
}

void login(char *message, int *client_socket_id) {
  char *text = strdup(message);

  char *username = strtok(text, "<>");
  char *password = strtok(NULL, "<>");
  password[strlen(password) - 1] = '\0'; // remove \n from a string

  if (isExistingUser(username, password)) { //if user is in database    
      char buffer[20] = {0};
    if (log_user(user_create(username, password), *client_socket_id)) { //put user in starting room
      strcpy(buffer, "Login successful\n");
      printf("Server is sending: '%s'(%ld)", buffer, strlen(buffer)); // Debug print
      write(*client_socket_id, buffer, strlen(buffer));               // Java recv needs string end with EOF
      printf("Login successful\n");

    } else { //unable to put it in starting room 
      strcpy(buffer, "Starting room full\n");
      write(*client_socket_id, buffer, strlen(buffer)); // Java recv need string end with EOF
      printf("Starting room full\n");
    }

  } else { //if user doesn't exist
    char buffer[20] = {0};
    strcpy(buffer, "Login failed\n");
    printf("Server is sending: '%s'(%ld)", buffer, strlen(buffer)); // Debug print
    write(*client_socket_id, buffer, strlen(buffer));               // Java recv need string end with EOF
    printf("Login failed\n");                                       // Server Log
  }

  free(text);
}


bool log_user(User *u, int client_socket_id) {
  //Log user into Starting room
  Client *client = client_create(u, client_socket_id, 0);
  Room *room_zero = get_room_by_id(0);
  bool status = room_add_client(room_zero, client); //room_add_client fails if room is full
  return status; 
}

void registerUser(char *message, int *client_socket_id) {
  char *text = strdup(message);

  char *username = strtok(text, "<>");
  char *password = strtok(NULL, "<>");
  password[strlen(password) - 1] = '\0'; // remove \n from string

  if (insertUser(username, password)) {
    char buffer[25] = {0};
    strcpy(buffer, "Register successful\n");
    printf("Server is sending: '%s'(%ld)", buffer, strlen(buffer)); // Debug print
    write(*client_socket_id, buffer, strlen(buffer));               // Java recv need string end with EOF
    printf("Register successful\n");
  } else {
    char buffer[25] = {0};
    strcpy(buffer, "Register failed\n");
    printf("Server is sending: '%s'(%ld)", buffer, strlen(buffer)); // Debug print
    write(*client_socket_id, buffer, strlen(buffer));               // Java recv need string end with EOF
    printf("Register failed\n");                                    // Server Log
  }

  free(text);
}

void getList(int *client_socket_id) {
  //Send list of current rooms 
  //To make sure client recieves entire list, we format the string sent this way:
  /*
  [LST]MAX_CLIENTS
  roomID<>roomNAME<>clientsConnected
  ...
  roomID<>roomNAME<>clientsConnected
  [/LST]

  */
  char start[10];
  char end[] = "[/LST]\n";
  char buff[42] = {0}; // formatted room buffer

  sprintf(start, "[LST]%d\n", MAX_CLIENTS);
  printf("Server is sending: '%s'(%ld)", start, strlen(start)); // Debug print
  write(*client_socket_id, start, strlen(start));

  int tmpFound = 0;                                  // how many rooms already found
  int i = 1;                                         // index to visit array (0 is tarting room so we exclude it)
  while (tmpFound < (rooms_getActive_rooms() - 1)) { // if all active rooms have been visited, we stop searching the array
    if (i > MAX_ROOMS) {                             // check to avoid seg fault
      perror("Qualcosa non va con l'array delle stanze");
    }
    get_formatted_room(i, buff); // it gets roomID<>roomNAME<>clientsConnected\n, if rooom is unactive termination character
    if (buff[0] != '\0') {
      printf("Server is sending: '%s'(%ld)", buff, strlen(buff)); // Debug print
      write(*client_socket_id, buff, strlen(buff));
      bzero(buff, 42);
      tmpFound++;
    } //if buff == '\0' room is unactive so we don't send it to client
    i++;
  }
  printf("Server is sending: '%s'(%ld)", end, strlen(end)); // Debug print
  write(*client_socket_id, end, strlen(end));
  printf("Room List sent successfully"); //Server Log
}