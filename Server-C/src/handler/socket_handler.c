#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "rooms_handler.h"
#include "socket_handler.h"
#include "../library/log.h"

void error_handler(char text[]);

//Main Dispatcher
bool socketDispatcher(int *client_socket_id, char *buffer) {

  char tag[6] = {0};
  strncpy(tag, buffer, 5);    // get the tag
  char *message = buffer + 5; // point to buffer without tag
  message = strdup(message);  // get rid of tag

  if (strncmp(tag, "[MSG]", 5) == 0) { // Message sent in room
    broadcast_message_into_room(&(*message), client_socket_id);
  } else if (strncmp(tag, "[LGN]", 5) == 0) { // Login user
    login(&(*message), client_socket_id);
  } else if (strncmp(tag, "[RGT]", 5) == 0) { // Register user
    register_user(&(*message), client_socket_id);
  } else if (strncmp(tag, "[CRT]", 5) == 0) { // Create room
    create_room(&(*message), client_socket_id);
    //print_rooms();                            // debug
  } else if (strncmp(tag, "[LST]", 5) == 0) { // Get List of room
    get_list(client_socket_id);
  } else if (strncmp(tag, "[RQT]", 5) == 0) { // Request from client to enter in a room
    request_to_enter_room(&(*message), client_socket_id);
  } else if (strncmp(tag, "[ACC]", 5) == 0) { // Accept user in a room
    accept_request(&(*message));
  } else if (strncmp(tag, "[NAC]", 5) == 0) { // Denied access in a room
    not_accept_request(&(*message));
  } else if (strncmp(tag, "[EXT]", 5) == 0) {
    if(!exit_room(&(*message), client_socket_id))
      return false; //if this function returns false, calling function will close socket
  } else {
    char text[] = "Message recieved doesn't contain valid tag\n";
    log_error("%s. Tag: %s, Message: %s",text, tag, message);
    if(write(*client_socket_id, text, strlen(text)) < 0)
      error_handler("Errore write");
  }
  return true;
}


//Request Processing
void broadcast_message_into_room(char *message, int *client_socket_id) {
  // Send message to every client in room

  char *message_to_send = strtok(message, "<>");
  char *string_room_id = strtok(NULL, "<>");

  int room_id = atoi(string_room_id);

  int length = strlen(message_to_send) + 1;
  char text[length + 15];

  Room *room = rooms_get_room_by_id(room_id);
  
  if(room == NULL){
    log_debug("Room to send message is NULL, stop broadcasting");
    //TODO: add client response, room is null
    return;
  }
    
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
        log_info("Server is sending(%ld): %s", strlen(text), text); // Debug print
        if(write(client_id, text, strlen(text)) < 0)
          error_handler("Errore write");

        count++;
      }
    }
  }
}


void login(char *message, int *client_socket_id) {
  char *text = strdup(message);

  char *username = strtok(text, "<>");
  char *password = strtok(NULL, "<>");
  char *status = "0";
  password[strlen(password) - 1] = '\0'; // remove \n from a string

  if (isExistingUser(username, password)) { //if user is in database and isn't logged //DA REVISIONARE
    if(isLoggedExistingUser(username, password)) {
      char buffer[20] = {0};
      
      if (log_user(user_create(username, password), *client_socket_id)) { //put user in starting room
        status = "1";
        dbUpdateStatus(username, status);
        strcpy(buffer, "Login successful\n");
        log_info("Server is sending(%ld): %s", strlen(buffer), buffer); // Debug print
        if(write(*client_socket_id, buffer, strlen(buffer)) < 0)               // Java recv needs string end with EOF
          error_handler("Error write");
      } else { //unable to put it in starting room 
        strcpy(buffer, "Starting room full\n");
        if(write(*client_socket_id, buffer, strlen(buffer)) < 0) // Java recv need string end with EOF
          error_handler("Error write");
        log_warn("Starting room full");
      }
    } else {
      //TODO: write "utente già loggato"
      log_info("User %s already logged", username);
    }
  } else { //if user doesn't exist
    char buffer[20] = {0};
    strcpy(buffer, "Login failed\n");
    log_info("Server is sending(%ld): %s", strlen(buffer), buffer); // Debug print
    if(write(*client_socket_id, buffer, strlen(buffer)) < 0)        // Java recv need string end with EOF
      error_handler("Errore write");
    log_warn("Login failed");                                       // Server Log
  }

  free(text);
}

void register_user(char *message, int *client_socket_id) {
  char *text = strdup(message);

  char *username = strtok(text, "<>");
  char *password = strtok(NULL, "<>");
  password[strlen(password) - 1] = '\0'; // remove \n from string

  if (insertUser(username, password)) {
    char buffer[25] = {0};
    strcpy(buffer, "Register successful\n");
    log_info("Server is sending(%ld): %s", strlen(buffer), buffer); // Debug print
    if( write(*client_socket_id, buffer, strlen(buffer)) < 0)
      error_handler("Errore write");               // Java recv need string end with EOF
  } else {
    char buffer[25] = {0};
    strcpy(buffer, "Register failed\n");
    log_info("Server is sending(%ld): %s", strlen(buffer), buffer); // Debug print
    if( write(*client_socket_id, buffer, strlen(buffer)) < 0)              // Java recv need string end with EOF
      error_handler("Errore write");
    log_warn("Register failed");                                    // Server Log
  }

  free(text);
}


void create_room(char *message, int *client_socket_id) {
  Client *client = rooms_get_client_from_room_by_id(0, *client_socket_id);
  if(client != NULL) {
    log_info("Getted user from room_zero with id: %d", *client_socket_id);
  }else{
    log_error("No user in room_zero with id: %d", *client_socket_id);
    char* text = "Error creation of room\n";
    log_warn("Server is sending(%ld): %s", strlen(text), text);
    if (write(*client_socket_id, text, strlen(text)) < 0)
      error_handler("Errore write");
    return;
  } 
  
  Room *room = NULL;

  if (*(message + strlen(message) - 1) == '\n') { // Remove '\n' from end of string that java puts
    *(message + strlen(message) - 1) = '\0';
    //log_debug("Removed newline from message");
  }
  
  room = room_create(0, message, client); //Create new room, id is set to 0 but will be changed
  log_info("Room created");
  
  if (rooms_add_room(room)) {
    log_debug("Room added");
    
    log_debug(room_to_string(room));
    char text[35];
    sprintf(text, "Room create successful<>%d\n", room->id);
    log_info("Server is sending(%ld): %s", strlen(text), text); // Debug print
    if(write(*client_socket_id, text, strlen(text)) < 0)               // Remember: Java recv need string end with EOF
      error_handler("Errore write");
    rooms_remove_from_zero(client->socket_id); // TODO: Ritorna un boolean, forse lo vogliamo gestire?
    return;
  }

  // else: failed to create room
  if(write(*client_socket_id, "Room create failed\n", 20) < 0) // Java recv need string end with EOF
    error_handler("Errore write");
  log_warn("Room create failed");
}

void get_list(int *client_socket_id) {
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
  log_info("Server is sending(%ld): %s", strlen(start), start); // Debug print
  if ( write(*client_socket_id, start, strlen(start)) < 0)
    error_handler("Errore write");

  unsigned int tmpFound = 0;                                  // how many rooms already found
  int i = 1;                                         // index to visit array (0 is tarting room so we exclude it)
  while (tmpFound < (rooms_active - 1)) { // if all active rooms have been visited, we stop searching the array
    if (i > MAX_ROOMS) {                             // check to avoid seg fault
      perror("Qualcosa non va con l'array delle stanze");
    }
    rooms_get_formatted_room(i, buff); // it gets roomID<>roomNAME<>clientsConnected\n, if rooom is unactive termination character
    if (buff[0] != '\0') {
      log_info("Server is sending(%ld): %s", strlen(buff), buff); // Debug print
      if(write(*client_socket_id, buff, strlen(buff)) < 0)
        error_handler("Errore write");
      bzero(buff, 42);
      tmpFound++;
    } //if buff == '\0' room is unactive so we don't send it to client
    i++;
  }
  log_info("Server is sending(%ld): %s", strlen(end), end); // Debug print
  if(write(*client_socket_id, end, strlen(end)) < 0)
    error_handler("Errore write");
  log_info("Room List sent successfully"); //Server Log
}


void request_to_enter_room(char *message, int *client_socket_id) {

  // client_socket_id is client requesting to enter room
  unsigned int room_id = atoi(message);
  printf("Room id chosen from client is: %d\n", room_id); // Debug print
  Room *room = rooms_get_room_by_id(room_id);
  Client *client = rooms_get_client_from_room_by_id(0, *client_socket_id);

  int master_client_socket_id = 0;

  //TODO: Decidere su questo controllo
  // if(room->master_client != NULL) //allo sto coso cerve a evitare il seg fault,
  // dovremmo valutare quanto vogliamo fare controlli
  // in teoira queste cose non dovrebbero succedere, ma di solito comunque si controlla

  master_client_socket_id = room->master_client->socket_id;
  char buffer[50];
  sprintf(buffer, "[RQT]%d<>%s<>%d\n", *client_socket_id, client->user->name, room_id);
  //[RQT]client_requesting_to_enter_socket_id<>user_name<>room_id
  log_info("Server is sending(%ld): %s", strlen(buffer), buffer); // Debug print
  if(write(master_client_socket_id, buffer, strlen(buffer)) < 0)
    error_handler("Errore write");
}

void accept_request(char *message) { // Accept user in a room
  // TODO: Rivedi i casi limiti strani

  char *string_socket_id_client = strtok(message, "<>");
  char *string_room_id = strtok(NULL, "<>");

  // Questo socket id è il client che è stato accettato

  int socket_id_client = atoi(string_socket_id_client);
  unsigned int room_id = atoi(string_room_id);

  // Insert client into room
  //TODO: controlla se questi diventano NULL
  Room *room = rooms_get_room_by_id(room_id);
  Client *client = rooms_get_client_from_room_by_id(0, socket_id_client);
  room_add_client(room, client);
  rooms_remove_from_zero(client->socket_id); // TODO: questo ritorna un boolean, vedere se lo vogliamo gestire

  // Debug prints: current and starting room
  room_print(room);
  room_print(rooms_get_room_by_id(0));

  // printf("Socket id client: %d \nRoom id: %d\n", socket_id_client, room_id);//Debug print

  // Send to Client it has been accepted
  char text[] = "Access accept\n";
  log_info("Server is sending(%ld): %s", strlen(text), text); // Debug print
  if(write(socket_id_client, text, strlen(text)) < 0)
    error_handler("Errore write");
}

void not_accept_request(char *message) { // Don't accept a Client in a room
  // Send to Client it has not been accepted
  unsigned int client_socket_id = atoi(message);
  char text[] = "Access denied\n";
  log_info("Server is sending(%ld): %s", strlen(text), text); // Debug print
  if(write(client_socket_id, text, strlen(text)) < 0)
    error_handler("Errore write");
}


bool exit_room(char* message, int *client_socket_id) { //Exit room
  int room_id = atoi(message);

  Client* client = rooms_get_client_from_room_by_id(room_id, *client_socket_id);
  if(client == NULL){
    log_error("Exit from client_socket_id:%d not found in room:%d", *client_socket_id, room_id);
    //TODO: write di "si è verificato un errore?" per il Client?
    return true;

  } //else

  if(room_id == 0){ //Disconnect from the app
    log_info("Socket %d is disconnecting from the app", *client_socket_id);
    
    //Room logic
    rooms_delete_client_from_room(*client_socket_id, 0);

    //Socket logic
    char message_to_send[] = "Disconnected";
    if(write(*client_socket_id, message_to_send, strlen(message_to_send)) < 0)
      error_handler("Errore write");
    return false; //when returning false, close soket

  } //else

  //Exiting from a room
  if (!rooms_move_to_zero(client, room_id)){
    //Unexpected behaviour
    log_warn("Could not move client with socket_id:%d out of room:%d", client_socket_id, room_id);
    //TODO: write di "si è verificato un errore?" per il Client?
  }else log_debug("Successfully moved client with socket_id:%d");

  //TODO: Aggiungere logica nel caso in cui è l'ultimo nella stanza, distruggerla
  if(rooms_is_empty(room_id)){
    log_debug("Room is empty, deleting room...");
    rooms_delete_room(room_id);
  }

  return true;
}


//Auxiliar
bool log_user(User *u, int client_socket_id) {
  //Log user into Starting room
  Client *client = client_create(u, client_socket_id, 0);
  Room *room_zero = rooms_get_room_by_id(0);
  bool status = room_add_client(room_zero, client); //room_add_client fails if room is full
  return status; 
}
/*
void error_handler(char text[]) { //TODO: decidere se usarlo con TUTTE le read/write
  log_error("%s", text);
  exit(EXIT_FAILURE);
}*/


