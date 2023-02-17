#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <pthread.h>
#include <signal.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>

#include "database/database.h"

#include "handler/rooms_handler.h"
#include "handler/socket_handler.h"

#include "objects/room.h" //room includes the other objects

#define PORT 9192

void error_handler(char[]);
void *socket_handler(void *);
void signal_handler(int);
void remove_client(int);

fd_set readfds, master;

int maxfdp;

int main(int argc, char const *argv[]) {

////INITIALIZATION SERVER////////////////////////////////////////////////
  signal(SIGINT, signal_handler); //Close database when server is stopped

  initDatabase();
  init_starting_room();
  char buffer[256] = {0};

  //Setup socket
  struct sockaddr_in saddress, caddress;
  int server_tcp, len;

  // Setup socket TCP SERVER
  saddress.sin_family = AF_INET;
  saddress.sin_addr.s_addr = inet_addr("127.0.0.1");
  saddress.sin_port = htons(PORT);

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
  printf("server_tcp_socket id: %d\n", server_tcp);
///SERVER INITIALIZED//////////////////////////////////////////////////


  while (1) {
    readfds = master;

    int nready = select(maxfdp + 1, &readfds, NULL, NULL, NULL); // Select get the highest socket ID + 1
    
    for (int i = 0; i <= maxfdp; i++) { //Check all File Descriptors 
      if (FD_ISSET(i, &readfds)) {      //to find which one changed

        if (i == server_tcp) { // If it's a New Connection

          printf("[TCP SOCKET ACTIVE - NEW CONNECTION] (%d)\n", i); //Log Server

          len = sizeof(caddress);
          int client_socket_id;
          if ((client_socket_id = accept(server_tcp, (struct sockaddr *)&caddress, &len)) == -1)
            error_handler("Accept Error");

          FD_SET(client_socket_id, &master);
          if (client_socket_id > maxfdp) 
            maxfdp = client_socket_id; //More clients need to be observed

        } else { // If it's an Existing Connetion
                 // new thread for every request
          printf("[TCP SOCKET ACTIVE - EXISTING CONNECTION] (%d)\n", i); //Server Log

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
  if (byte <= 0) { //If Client Disconnects or Error occurs:
    printf("Client disconnected [%d]\n", client_socket_id); //Server Log
    close(client_socket_id);
    FD_CLR(client_socket_id, &master); 
    // TODO: controllare se si deve fare altro per la disconnessione!
    if(client_setSocket_id == maxfdp)
      maxfdp--;
    //  TODO : togliere il client dalla stanza in cui stava! [Funzione ancora da implementare]
  } else {//fulfill client request
    printf("Message received: %s\n", buffer);
    socketDispatcher(&client_socket_id, buffer);
  }

  printf("\n\t THREAD FINISH \n");
  pthread_exit(NULL);
}

void error_handler(char text[]) { //TODO: decidere se usarlo con TUTTE le read/write
  perror(text);
  exit(EXIT_FAILURE);
}

void signal_handler(int sig) {
  closeDatabase();
  exit(EXIT_SUCCESS);
}
