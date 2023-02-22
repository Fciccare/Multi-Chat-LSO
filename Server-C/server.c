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


#include "database/database.h"

#include "handler/rooms_handler.h"
#include "handler/socket_handler.h"

#include "objects/room.h" //room includes the other objects

#include "library/log.h"

void error_handler(char[]);
void *socket_handler(void *);
void signal_handler(int);
void remove_client(int);

void test();

fd_set readfds, master;

int maxfdp;

int main(int argc, char* argv[]) {

////INITIALIZATION SERVER////////////////////////////////////////////////
  signal(SIGINT, signal_handler); //Close database when server is stopped

  char buffer[256] = {0};
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
  log_debug("Room initiated");

  //Setup socket
  struct sockaddr_in saddress, caddress;
  int server_tcp, len;

  // Setup socket TCP SERVER
  saddress.sin_family = AF_INET;
  saddress.sin_addr.s_addr = inet_addr("127.0.0.1");
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

    int nready = select(maxfdp + 1, &readfds, NULL, NULL, NULL); // Select get the highest socket ID + 1
    
    for (int i = 0; i <= maxfdp; i++) { //Check all File Descriptors 
      if (FD_ISSET(i, &readfds)) {      //to find which one changed

        if (i == server_tcp) { // If it's a New Connection

          log_info("SOCKET ACTIVETED IS: %d", i); //Log Server

          len = sizeof(caddress);
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
  if (byte <= 0) { //If Client Disconnects or Error occurs:
    log_info("SOCKET CLIENT DISCONNECTED ID: %d", client_socket_id); //Server Log
    close(client_socket_id);
    FD_CLR(client_socket_id, &master); 
    // TODO: controllare se si deve fare altro per la disconnessione!
    if(client_socket_id == maxfdp)
      maxfdp--;
    //  TODO : togliere il client dalla stanza in cui stava! [Funzione ancora da implementare]
  } else {//fulfill client request
    log_info("Message received: %s", buffer);
    socketDispatcher(&client_socket_id, buffer);
  }

  log_debug("THREAD FINISH");
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

// void test() { //è uno schifo ma non cancelliamolo
//   log_info("INIZIO TESSSSSSTTTTTTTT");

//   int *p1, *p2, *p3;
//   int s1 = 4; 
//   int s2 = 5;
//   int s3 = 6;

//   p1=&s1;
//   p2=&s2;
//   p3=&s3;

//   if(isExistingUser("a", "a")){
//     User* u1 = user_create("a","a");
//     log_user(u1, *p1);
//   }

//   if(isExistingUser("user1", "password")){
//     User* u2 = user_create("user1","password");
//     log_user(u2, *p2);
//   }

//   if(isExistingUser("Girgio", "Giorgio")){
//     User* u3 = user_create("Girgio","Giorgio");
//     log_user(u3, *p3);
//   }

//   // room = room_create(0, message, client); //Create new room, id is set to 0 but will be changed
//   // log_info("Room created");
  
//   // if (rooms_add_room(room)) {
//   //   log_debug("Room added");
    
//   //   room_print(room);//TODO: Change with room_to_string for prevent spam room null
//   //   char text[35];
//   //   sprintf(text, "Room create successful<>%d\n", room->id);
//   //   log_info("Server is sending(%ld): %s", strlen(text), text); // Debug print
//   //   write(*client_socket_id, text, strlen(text));               // Remember: Java recv need string end with EOF
//   //   rooms_remove_from_zero(client->socket_id); // TODO: Ritorna un boolean, forse lo vogliamo gestire?
//   //   return;

//   //Dalla funzione accept_request
//   Room *room = rooms_get_room_by_id(1);
//   Client *client = rooms_get_client_by_id(*p2);
//   room_add_client(room, client);
//   rooms_remove_from_zero(client->socket_id);

//   client = rooms_get_client_by_id(*p3);
//   room_add_client(room, client);
//   rooms_remove_from_zero(client->socket_id);
//   ///////////////////////////////

//   room_remove_client(room, *p2);
//   rooms_move_to_zero(client);

// }
