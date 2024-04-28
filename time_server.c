#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>


#define MAX_BUFFER_SIZE 256

int num_process = 8;

int check(char *msg) {
    char cmd[20];
   
    if (sscanf(msg, "GET_TIME %s", cmd) == 1) {
        char *format[] = {"dd/mm/yyyy", "dd/mm/yy", "mm/dd/yyyy", "mm/dd/yy"};
        for (int i = 0; i < 4; i++) {
            if (strcmp(cmd, format[i]) == 0) {
                return i; 
            }
        }
    }
    return -1;
}

char *getTime(int i) {
    char *format[] = {"%d/%m/%Y", "%d/%m/%y", "%m/%d/%Y", "%m/%d/%y"};
    char *buffer = (char *)malloc(20 * sizeof(char));
    time_t currentTime;
    struct tm *localTime;

    time(&currentTime);                  
    localTime = localtime(&currentTime);

    strftime(buffer, 20, format[i], localTime); 

    return buffer; 
}

void processtt(int client) {
    char buff[MAX_BUFFER_SIZE];
    while (1) {
        memset(buff, 0, MAX_BUFFER_SIZE);
        int ret = recv(client, buff, sizeof(buff), 0);
        if (ret < 0) {
            continue;
        }
        //printf("%s\n",buff);
        if (check(buff) == -1) {
            char msgg[] = "Vui long nhap dung format !";
            send(client, msgg, strlen(msgg), 0);
        } else {
            int k = check(buff);
            //printf("abcccc\n");
            char *msggg = getTime(k);
            send(client, msggg, strlen(msggg), 0);
            free(msggg);
        }
    }
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
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork() failed");
            return 1;
        } else if (pid == 0) {
            while (1) {
                int client = accept(server, NULL, NULL);
                if (client == -1) {
                    perror("accept() failed");
                    exit(1);
                }
                processtt(client);
            }
            exit(0);
        }
    }

    getchar();
    kill(0, SIGKILL);

    close(server);
    return 0;
}
