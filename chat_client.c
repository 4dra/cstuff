#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
 
#define PORT 8060
#define MSG_LIM 256

void chat(int);

void error(const char* msg) {
    perror(msg);
    exit(1);
}

int main(int argc,char **argv) {
    struct sockaddr_in serv_addr;
    int sockfd;
    struct hostent *server;
 
    bzero(&serv_addr, sizeof serv_addr);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server = gethostbyname(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
 
    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
        error(NULL);

    printf("INF: connection established\n");
    chat(sockfd);
    
    return 0;
}

void chat(int socket) {
    char buffer[MSG_LIM];

    bzero(buffer, MSG_LIM);
    fgets(buffer, MSG_LIM - 1, stdin);
    write(socket, buffer, strlen(buffer) + 1);
    
    bzero(buffer, MSG_LIM);
    read(socket, buffer, MSG_LIM - 1);
    printf("server: %s\n", buffer);
}
