#include <arpa/inet.h>
#include <fcntl.h> 
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>  
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>

#include "struct/struct.h"
#include "handler/socket_handler.h"

#define PORT 9192

void error_handler(char[]);
void *socket_handler(void *);
void signal_handler(int);
void remove_client(int);

int count_client = 0;
int clients[1024] = {0};
fd_set readfds, master;

int main(int argc, char const *argv[]) {
    signal(SIGINT, signal_handler);
    char buffer[256] = {0};
    char *message = "Hello I'm server";
    struct sockaddr_in saddress, caddress;
    int server_tcp, len, client;

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

    /*----------------------------------------*/
    /*----------------------------------------*/
    /*----------------------------------------*/
    initRoom();
    Room room1 = {"Gatti", 0, NULL, 0};
    Room room2 = {"Cani", 0, NULL, 0};
    addRoom(&room1);
    addRoom(&room2);
    printRooms();
    /*----------------------------------------*/
    /*----------------------------------------*/
    /*----------------------------------------*/

    FD_ZERO(&master);
    FD_ZERO(&readfds);
    FD_SET(0, &master);
    FD_SET(server_tcp, &master);

    int maxfdp = server_tcp;
    printf("server_tcp_socket id: %d\n", server_tcp);
    while (1) {
        readfds = master;

        int nready = select(maxfdp + 1, &readfds, NULL, NULL, NULL);  // Select get the highest socket ID + 1.
         
        for (int i = 0; i <= maxfdp; i++) {
            if (FD_ISSET(i, &readfds)) {
                if (i == server_tcp) {  // new Connection
                    printf("[TCP SOCKET ACTIVE - NEW CONNECTION] (%d)\n", i);
                    len = sizeof(caddress);
                    client = accept(server_tcp, (struct sockaddr *)&caddress, &len);
                    // fcntl(client, F_SETFL, O_NONBLOCK);//non blocking
                    clients[count_client++] = client;
                    FD_SET(client, &master);
                    if (client > maxfdp) maxfdp = client;
                } else {  // existing connetion
                    printf("[TCP SOCKET ACTIVE - EXISTING CONNECTION] (%d)\n", i);
                    // socket_handler((void*)&i);
                    pthread_t thread_id;
                    if (pthread_create(&thread_id, NULL, socket_handler, (void *)&i) < 0)
                        error_handler("Errore creazione socket");
                    pthread_join(thread_id, NULL);
                    // pthread_detach(thread_id);
                }
            }
        }
    }
    return 0;
}

void *socket_handler(void *client_void) {//passare a un puntatore e non a una copia
    int client = *(int *)client_void;
    char buffer[256] = {0};
    int byte = read(client, buffer, sizeof(buffer));
    pthread_detach(pthread_self());  // Stacco in modo che il thread venga deallocato
    if (byte <= 0) {
        printf("Client disconnected [%d]\n", client);
        close(client);
        FD_CLR(client, &master);//da aggiustare poiché fondamentale per la disconnesione
        //TODO: Decrementare maxfds
        // TODO GESTIRE MEGLIO L'ARRAY DEI CLIENT
    } else printf("Message received: %s\n", buffer);

    socketDispatcher(&client, buffer, clients); //clients is only for legacy function

    printf("\n\n\t THREAD FINISH \n\n");
    pthread_exit(NULL);
}

// void remove_client(int client){
//     for(int k = 0; k < count_client; ++k){
//         if(clients[k] == client){
//             printf("Removed %d from client\n", client);
//             clients[k] = 0;
//         }
//     }
//     for (int i = 0; i < count_client; i++) {
//         printf("[%d]: %d\t", i, clients[i]);
//     }
// }

void error_handler(char text[]) {
    perror(text);
    exit(EXIT_FAILURE);
}

void signal_handler(int sig) { 
    exit(EXIT_SUCCESS);
}
