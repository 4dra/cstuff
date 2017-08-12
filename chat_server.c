#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>

#define CLIENTS_LIM 5
#define PORT 8060
#define MSG_LIM 256

int main() {
    struct sockaddr_in serv_addr, cli_addr;
    int pid, sockfd, comfd;
    char buffer[MSG_LIM];

    signal(SIGCHLD, SIG_IGN);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));

    listen(sockfd, CLIENTS_LIM);
    comfd = accept(sockfd, (struct sockaddr*) NULL, NULL);
    
    while (1) {
        
        bzero(buffer, MSG_LIM);
        read(comfd, buffer, MSG_LIM - 1);
        
        printf("client: %s",buffer);

        char response[] = "fu";

        write(comfd, response, strlen(response) + 1);
    }

    close(comfd);
    return 0;
}
