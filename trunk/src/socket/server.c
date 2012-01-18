#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define MAXPENDING 5    /* Max connection requests */
#define BUFFSIZE 1024
void Die(char *mess) { perror(mess); exit(1); }


int main(int argc, char *argv[]) {
    int serversock, clientsock;
    struct sockaddr_in echoserver, echoclient;
    char buffer[BUFFSIZE];

    if (argc != 2) {
      fprintf(stderr, "USAGE: echoserver <port>\n");
      exit(1);
    }
    /* Create the TCP socket */
    if ((serversock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      Die("Failed to create socket");
    }
    /* Construct the server sockaddr_in structure */
    memset(&echoserver, 0, sizeof(echoserver));       /* Clear struct */
    echoserver.sin_family = AF_INET;                  /* Internet/IP */
    echoserver.sin_addr.s_addr = htonl(INADDR_ANY);   /* Incoming addr */
    echoserver.sin_port = htons(atoi(argv[1]));       /* server port */
    
    /* Bind the server socket */
    if (bind(serversock, (struct sockaddr *) &echoserver,
                               sizeof(echoserver)) < 0) {
    Die("Failed to bind the server socket");
    }
    /* Listen on the server socket */
    if (listen(serversock, MAXPENDING) < 0) {
    Die("Failed to listen on server socket");
    }
    
    /* Run until cancelled */
    while (1) {
      unsigned int clientlen = sizeof(echoclient);
      /* Wait for client connection */
      if ((clientsock =
           accept(serversock, (struct sockaddr *) &echoclient,
                  &clientlen)) < 0) {
        Die("Failed to accept client connection");
      }
      printf("Client connected: %s\n",
                      inet_ntoa(echoclient.sin_addr));

      
      recv(clientsock, buffer, BUFFSIZE, 0);
      printf("Recu : %s \n", buffer);
      // Instructions to CAN

    }
}
