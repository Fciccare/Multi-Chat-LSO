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
void socket_close(int);


void error_handler(char[]);
void signal_handler();

fd_set readfds, master;
int maxfdp;

int main(int argc, char* argv[]) {

////INITIALIZATION SERVER////////////////////////////////////////////////
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
  init_starting_room();

  //Setup socket
  struct sockaddr_in saddress, caddress;
  int server_tcp;

  // Setup socket TCP SERVER
  saddress.sin_family = AF_INET;
  saddress.sin_addr.s_addr = inet_addr(ipaddr);
  saddress.sin_port = htons(port);

  // Creation socket TCP SERVER
  if ((server_tcp = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    error_handler("Errore socket");
  if (setsockopt(server_tcp, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
    error_handler("Errore setsockopt");
  if (bind(server_tcp, (struct sockaddr *)&saddress, sizeof(saddress)) < 0)
    error_handler("Errore bind");
  if (listen(server_tcp, 5) < 0)
    error_handler("Errore listen");

  FD_ZERO(&master);
  FD_ZERO(&readfds);
  FD_SET(0, &master);
  FD_SET(server_tcp, &master);

  maxfdp = server_tcp;
  log_info("MASTER SOCKET SUCCEFULL CREATED WITH ID: %d", server_tcp);
///SERVER INITIALIZED//////////////////////////////////////////////////

  // test();

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
            error_handler("Accept Error");
          
          FD_SET(client_socket_id, &master);
          if (client_socket_id > maxfdp) 
            maxfdp = client_socket_id; //More clients need to be observed
          log_info("NEW CLIENT SOCKET ADDED WITH ID: %d", client_socket_id);

        } else { // If it's an Existing Connetion
                 // new thread for every request
          log_info("SOCKET EXISTING CONNECTION (%d)", i); //Server Log

          pthread_t thread_id;
          if (pthread_create(&thread_id, NULL, socket_handler, (void *)&i) < 0) 
            error_handler("Errore creazione thread");
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

  pthread_detach(pthread_self()); // We detach here so that thread can be deallocated when finished
  
  if (byte <= 0) { //Client Disconnects or Error occurs
    log_info("SOCKET CLIENT DISCONNECTED ID: %d", client_socket_id); //Server Log
    
    //Socket logic
    socket_close(client_socket_id);
  
    Client* client = rooms_get_client_by_id(client_socket_id);
    if(client == NULL){ 
      //Not logged user
      log_debug("Client doesn't exist. ID: %d. Just disconnecting", client_socket_id);
    
    } else { //Logged user
      //DB logic
      log_debug("Updating DB status of client with socket id: %d", client_socket_id);
      dbUpdateStatus(client->user->name, "0");

      //Rooms logic
      rooms_remove_destroy_client(client);
    }

  } else { //Client has request
    log_info("Message received: %s", buffer);

    if (!socketDispatcher(&client_socket_id, buffer)){ //fulfill request
      //if soscketDispatcher returns false, invalid request: close socket
      socket_close(client_socket_id);
    }
  }

  log_debug("THREAD FINISH");
  pthread_exit(NULL);
}

void socket_close(int client_socket_id) { //Auxiliar function to close a socket connection
  close(client_socket_id);
  FD_CLR(client_socket_id, &master);
  if(client_socket_id == maxfdp)
    maxfdp--;
}


void error_handler(char text[]) { //TODO: decidere se usarlo con TUTTE le read/write
  log_error("%s", text);
  exit(EXIT_FAILURE);
}

void signal_handler() {
  closeDatabase();
  //TODO: rooms_destroy();
  exit(EXIT_SUCCESS);
}

