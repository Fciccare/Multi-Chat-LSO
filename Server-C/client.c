#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#define PORT 9192

void error_handler(char[]);
void signal_handler(int sig);
void *socket_print_handler(void *);
int client;

int main(int argc, char const *argv[]){
    signal(SIGINT, signal_handler);
    char buffer[256] = {0};
    struct sockaddr_in address;
    char *message = "I want create connection";

    // Setup socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(PORT);

    if ((client = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error_handler("Errore socket");

    if (connect(client, (struct sockaddr *)&address, sizeof(address)) < 0)
        error_handler("Errore connect");


    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, socket_print_handler, (void *)&client) < 0)
        error_handler("Errore creazione socket");
    pthread_detach(thread_id); 

    while(1){
        bzero(buffer,256);
        // strcpy(buffer, "Messaggio di test");
        fgets(buffer,256,stdin);
        if (write(client, (char *)buffer, 256) < 0)
            error_handler("Errore write");
        
        sleep(1);
    }
    
}

void *socket_print_handler(void *client_void) {//Migliorare tutto il thread
    int client = *(int *)client_void;
    char buffer[256] = {0};
    while(1){
        bzero(buffer, 256);
        if (read(client, buffer, 256) < 0) error_handler("Errore write");
        printf("> %s \n", buffer);
        //TODO: MIGLIORARE LA CLOSE
    }
    pthread_exit(NULL);
}

void error_handler(char text[]){
    perror(text);
    exit(EXIT_FAILURE);
}

void signal_handler(int sig){
    // Close e clean socket
    if (close(client) < 0) error_handler("Errore close client");
}
