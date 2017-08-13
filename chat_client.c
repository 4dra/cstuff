#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
 
#define PORT 8060
#define MSG_LIM 256

void* read_messages(void*);

void error(const char* msg) {
    perror(msg);
    exit(1);
}

int main(int argc,char **argv) {
    struct sockaddr_in serv_addr;
    int sockfd;
    struct hostent *server;
    char buffer[MSG_LIM];
    pthread_t tid;
 
    bzero(&serv_addr, sizeof serv_addr);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server = gethostbyname(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
 
    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
        error(NULL);

    printf("SYS: connection established\n");

    pthread_create(&tid, NULL, &read_messages, (void*)(intptr_t)sockfd);
    
    while (1) {
        bzero(buffer, MSG_LIM);
        fgets(buffer, MSG_LIM - 1, stdin);
        write(sockfd, buffer, strlen(buffer) + 1);    
    }

    return 0;
}

void* read_messages(void* sockfdp) {
    int sockfd = (intptr_t)sockfdp;
    char buffer[MSG_LIM];
    int n;

    while (1) {
        bzero(buffer, MSG_LIM);
        n = read(sockfd, buffer, MSG_LIM - 1);
        
        if (n <= 0) {
            printf("SYS: disconnected by server\n");
            close(sockfd);
            exit(1);
        }
        
        printf("%s", buffer);
    }
}
