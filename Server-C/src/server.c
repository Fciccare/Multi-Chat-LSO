#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include <fcntl.h>
#include <pthread.h>
#include <signal.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>

//#include "database/database.h"

#include "handler/rooms_handler.h"
#include "handler/socket_handler.h"

#include "objects/room.h" //room includes the other objects

#include "library/log.h"


void *socket_handler(void *);
void close_socket(int socket_id, int status);
void close_all_socket();
void close_server();

void fatal_error_handler(char[]);
// void fatal_error_handler(char[]);
void signal_handler();

fd_set readfds, master;
int maxfdp;

int server_tcp;


int main(int argc, char* argv[]) {

///SERVER INITIALIZATION////////////////////////////////////////////////
  signal(SIGINT, signal_handler); //Close database when server is stopped

  //char buffer[256] = {0};
  int opt, port = 9192;
  char ipaddr[16] = "127.0.0.1";
  bool debug = false;
  log_set_level(2);

  //get arguments from command line
  while ((opt = getopt (argc, argv, "i:p:dh")) != -1){ //i need argument , p need argument, d no arg, h no arg
    switch(opt){
      case 'd':
        log_set_level(1);
        debug = true;
      break;
      case 'h':
        printf("Command line arguments:\n -h for show this\n -d for logging to stdout \n -i for set ip address (default: localhost)\n -p for set port (defualt: 9192)\n");
        exit(EXIT_SUCCESS);
      case 'p':
        port = atoi(optarg);
      break;
      case 'i':
        strcpy(ipaddr, optarg);
      break;
      case '?': //when i or p don't have arguments
        exit(EXIT_FAILURE);
    }
  }

  log_info("Server address: %s, Port: %d", ipaddr, port);
  //Init database and structure
  initDatabase(debug);
  rooms_init();

  //Setup socket
  struct sockaddr_in saddress, caddress;

  // Setup socket TCP SERVER
  saddress.sin_family = AF_INET;
  saddress.sin_addr.s_addr = inet_addr(ipaddr);
  saddress.sin_port = htons(port);

  // Creation socket TCP SERVER
  if ((server_tcp = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    fatal_error_handler("Errore socket");
  if (setsockopt(server_tcp, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
    fatal_error_handler("Errore setsockopt");
  if (bind(server_tcp, (struct sockaddr *)&saddress, sizeof(saddress)) < 0)
    fatal_error_handler("Errore bind");
  if (listen(server_tcp, 5) < 0)
    fatal_error_handler("Errore listen");

  FD_ZERO(&master);
  FD_ZERO(&readfds);
  FD_SET(0, &master);
  FD_SET(server_tcp, &master);

  maxfdp = server_tcp;
  log_info("MASTER SOCKET SUCCEFULL CREATED WITH ID: %d", server_tcp);
///SERVER INITIALIZED//////////////////////////////////////////////////

  while (1) {
    readfds = master;

    /*int nready = */select(maxfdp + 1, &readfds, NULL, NULL, NULL); // Select get the highest socket ID + 1
    
    for (int i = 0; i <= maxfdp; i++) { //Check all File Descriptors 
      if (FD_ISSET(i, &readfds)) {      //to find which one changed

        if (i == server_tcp) { // If it's a New Connection

          log_info("SOCKET ACTIVETED IS: %d", i); //Log Server

          unsigned int len = sizeof(caddress);
          int client_socket_id;
          if ((client_socket_id = accept(server_tcp, (struct sockaddr *)&caddress, &len)) == -1)
            fatal_error_handler("Accept Error");
          
          FD_SET(client_socket_id, &master);
          if (client_socket_id > maxfdp) 
            maxfdp = client_socket_id; //More clients need to be observed
          log_info("NEW CLIENT SOCKET ADDED WITH ID: %d", client_socket_id);

        } else { // If it's an Existing Connetion
                 // new thread for every request
          log_info("SOCKET EXISTING CONNECTION (%d)", i); //Server Log

          pthread_t thread_id;
          if (pthread_create(&thread_id, NULL, socket_handler, (void *)&i) < 0) 
            fatal_error_handler("Errore creazione thread");
          pthread_join(thread_id, NULL);
        }
      }
    }
  }
  return 0;
}

void *socket_handler(void *client_socket_id_void) { // passare a un puntatore e non a una copia
  int client_socket_id = *(int *)client_socket_id_void;
  char buffer[256] = {0};
  int byte = read(client_socket_id, buffer, sizeof(buffer));
  if(byte < 0)
    fatal_error_handler("Errore read");

  pthread_detach(pthread_self()); // We detach here so that thread can be deallocated when finished
  
  //CLIENT DISCONNECT OR ERROR OCCURRS
  if (byte <= 0) { 
    log_info("SOCKET CLIENT DISCONNECTED ID: %d", client_socket_id); //Server Log
    
    //Socket logic
    close_socket(client_socket_id, byte);
  
    //Rooms logic
    Client* client = rooms_get_client_by_id(client_socket_id);


    //Not logged user
    if(client == NULL){ 
      log_debug("Client doesn't exist. ID: %d. Just disconnecting", client_socket_id);
    
    //Logged user
    } else {
      //DB logic
      log_debug("Updating DB status of client with socket id: %d", client_socket_id);
      dbUpdateStatus(client->user->name, "0");

      int room_id = client->room_id;
      char username[32];
      strcpy(username, client->user->name);

      //Rooms logic
      int status = rooms_remove_destroy_client(client); //Returns: -2 on error, -1 if room was deleted, room_id if master changed and 0 if has not
      if(status < -2){
        log_warn("Error removing and destroying client from room");
      }
      
      if(status > 0 && room_id != 0){
        log_info("New master in room with id: %d", room_id);
        notify_new_master(room_id);
      }

      //Notify other clients in room
      if(room_id != 0 && status > -1){
        //char buffer[100];
        sprintf(buffer, "[MSG]L'utente %s è uscito/a dalla stanza<>%d\n", username, room_id);
        int admin_socket=0;
        broadcast_message_into_room(buffer, &admin_socket);
      }
    } 

  //CLIENT HAS REQUESTED SOMETHING
  } else { 
    log_info("Message received: %s", buffer);

    if (!socketDispatcher(&client_socket_id, buffer)){ //fulfill request
      //if soscketDispatcher returns false, invalid request or error occurred close socket

      close_socket(client_socket_id, -1);
    }
  }

  log_debug("THREAD FINISH");
  pthread_exit(NULL);
}

void close_socket(int client_socket_id, int status) { //Auxiliar function to close a socket 
  if(status < 0 && client_socket_id > server_tcp){ //When error occurs or we're forcing a disconnection (if it's the server socket we don't want to send a message)
    char buffer[100];
    sprintf(buffer, "[EXT]Errore nella comunicazione con il server, disconnessione...\n");
    if(write(client_socket_id, buffer, strlen(buffer)) < 0)
      fatal_error_handler("Errore write");
      
  } //else we are closing the socket because the client has disconnected
  
  close(client_socket_id);
  FD_CLR(client_socket_id, &master);
  if(client_socket_id == maxfdp)
    maxfdp--;
}

void close_all_socket() { //Auxiliar function to close all sockets
  for (int i = 0; i <= maxfdp; i++) {
    if (FD_ISSET(i, &master)) {
      close_socket(i, -1); //We're forcing the disconnection of all clients
    }
  }
}

void close_server(){
  log_info("Closing server");
  close_all_socket();
  closeDatabase();
  rooms_destroy();
  log_info("Server closed");
  exit(EXIT_SUCCESS);
}


void ffatal_error_handler(char text[]) {
  log_error("A fatal error has occurred: %s", text);
  close_server();
  exit(EXIT_FAILURE);
}

void fatal_error_handler(char text[]) {
  log_error("An error has occurred: %s", text);
}

void signal_handler() {
  log_info("Signal received");
  close_server();
  exit(EXIT_SUCCESS);
}
