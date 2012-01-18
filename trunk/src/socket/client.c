#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

void Die(char *mess) { perror(mess); exit(1); }


/* Called when CAN reception */

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in echoserver;
    unsigned int echolen;


    if (argc != 4) {
      fprintf(stderr, "USAGE: client <server_ip> <word> <port>\n");
      exit(1);
    }
    /* Create the TCP socket */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      Die("Failed to create socket");
    }
    
    /* Construct the server sockaddr_in structure */
    memset(&echoserver, 0, sizeof(echoserver));       /* Clear struct */
    echoserver.sin_family = AF_INET;                  /* Internet/IP */
    echoserver.sin_addr.s_addr = inet_addr(argv[1]);  /* IP address */
    echoserver.sin_port = htons(atoi(argv[3]));       /* server port */
    /* Establish connection */
    if (connect(sock,
                (struct sockaddr *) &echoserver,
                sizeof(echoserver)) < 0) {
      Die("Failed to connect with server");
    }
    
  
    /* Send the word to the server */
    echolen = strlen(argv[2]);
    if (send(sock, argv[2], echolen, 0) != echolen) {
      Die("Mismatch in number of sent bytes");
    }

    close(sock);
    exit(0);
}
