#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#define CLIENTS_LIM 5
#define PORT 8060
#define MSG_LIM 256

void* read_messages(void*);
void write_to_peers(int, char*); 

void error(const char* msg) {
    perror(msg);
    exit(1);
}

int CLIENTS[2] = { -1, -1 };

int main() {
    struct sockaddr_in serv_addr, cli_addr;
    int pid, sockfd, comfd;
    char buffer[MSG_LIM];
    pthread_t tid;

    signal(SIGCHLD, SIG_IGN);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));

    listen(sockfd, CLIENTS_LIM);

    while (1) {
        comfd = accept(sockfd, (struct sockaddr*) NULL, NULL);
        
        bzero(buffer, MSG_LIM);
        sprintf(buffer, "SYS: client %d connected.\n", comfd);
        printf("%s", buffer);

        if (CLIENTS[0] == -1) {
            CLIENTS[0] = comfd;
        } else if (CLIENTS[1] == -1) {
            CLIENTS[1] = comfd;
        } else {
            printf("SYS: server full, client denied.\n");
            close(comfd);
            continue;
        }

        write_to_peers(comfd, buffer);

        pthread_create(&tid, NULL, &read_messages, (void*)(intptr_t)comfd);
    }

    close(comfd);

    return 0;
}

void* read_messages(void* comfdp) {
    int comfd = (intptr_t)comfdp;
    char buffer[MSG_LIM], res_buffer[MSG_LIM];
    int n;
    
    while (1) {
        bzero(res_buffer, MSG_LIM);
        bzero(buffer, MSG_LIM);

        n = read(comfd, buffer, MSG_LIM - 1);
        
        if (n <= 0) {
            sprintf(res_buffer, "SYS: client %d disconnected.\n", comfd);
            printf("%s", res_buffer);

            write_to_peers(comfd, res_buffer);

            if (comfd == CLIENTS[0]) 
                CLIENTS[0] = -1;
            else if (comfd == CLIENTS[1])
                CLIENTS[1] = -1;

            close(comfd);
            pthread_exit(NULL);
        }

        sprintf(res_buffer, "[client %d]: %s", comfd, buffer);
        printf("%s", res_buffer);

        write_to_peers(comfd, res_buffer);
    }

    close(comfd);
}

void write_to_peers(int comfd, char* msg) { 
    for (int i = 0; i < 2; i++) {
        if (comfd != CLIENTS[i]) {
            write(CLIENTS[i], msg, strlen(msg) + 1);
        }
    }
}
