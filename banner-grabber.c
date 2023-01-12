#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFSIZE 1024

int main(int argc, char *argv[]) {
    char *host, *port;
    struct hostent *server;
    struct sockaddr_in serv_addr;
    int sockfd, n;
    char buffer[BUFSIZE];

    if (argc < 3) {
        fprintf(stderr, "Usage: %s host port\n", argv[0]);
        exit(1);
    }
    host = argv[1];
    port = argv[2];

    // Create a new socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        exit(1);
    }

    // Get the server's address
    server = gethostbyname(host);
    if (server == NULL) {
        fprintf(stderr, "Error: no such host\n");
        exit(1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(atoi(port));

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting to server");
        exit(1);
    }

    // Send the request to the server
    char *request = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
    write(sockfd, request, strlen(request));

    // Read the response from the server
    bzero(buffer, BUFSIZE);
    n = read(sockfd, buffer, BUFSIZE - 1);
    if (n < 0) {
        perror("Error reading from server");
        exit(1);
    }

    // Print the response
    printf("%s\n", buffer);

    // Close the socket
    close(sockfd);

    return 0;
}
