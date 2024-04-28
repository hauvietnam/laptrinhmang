#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int num_process = 8;

void tt_conProcerss(int client) {
    char buff[1024];
    int ret = recv(client, buff, sizeof(buff), 0);
    if (ret < 0) {
        perror("recv() failed");
        close(client);
        return;
    }
    buff[ret] = '\0';
    printf("Received message from client %d: %s\n", client, buff);

    char *msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello, World!</h1></body></html>";
    send(client, msg, strlen(msg), 0);
    close(client);
}

int main() {
    int server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == -1) {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    if (bind(server, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind() failed");
        return 1;
    }

    if (listen(server, 5) == -1) {
        perror("listen() failed");
        return 1;
    }

    for (int i = 0; i < num_process; i++) {
        if (fork() == 0) {
            // tt con
            while (1) {
                int client = accept(server, NULL, NULL);
                if (client == -1) {
                    perror("accept() failed");
                    exit(1);
                }
                tt_conProcerss(client);
            }
        }
    }

   
    // for (int i = 0; i < MAX_CLIENTS; i++) {
    //     wait(NULL); 
    // }
    wait(NULL); 

    close(server);
    return 0;
}
