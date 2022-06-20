#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <wait.h>

int calc(int num1, int num2, int operation) {
    int result;
    if (operation == 1) {
        result = num1 + num2;
    } else if (operation == 2) {
        result = num1 - num2;
    } else if (operation == 3) {
        result = num1 * num2;
    } else {
        result = num1 / num2;
    }
    return result;
}

void clientHandler(int sig) {
    if (fork() == 0) {
        int fd = open("to_srv", 'r');
        if (fd == -1){
            exit(0);
        }
        char input[100], c;
        int num1, num2, operation, clientPid, pidLen, i;
        for (i = 0; read(fd, &c, 1) > 0 ; ++i) {
            input[i] = c;
        }
        close(fd);
        remove("to_srv");
        char* token = NULL;
        token = strtok(input, " ");
        char path[100];
        strcpy(path, "to_client_");
        char clientPid_string[10];
        strcat(path, token);
        clientPid = atoi(token);
        token = strtok(NULL, " ");
        num1 = atoi(token);
        token = strtok(NULL, " ");
        operation = atoi(token);
        token = strtok(NULL, "");
        num2 = atoi(token);
        if (num2 == 0 && operation == 4) {
            kill(clientPid, SIGILL);
            exit(0);
        }
        int result = calc(num1, num2, operation);
        char answer[100];
        sprintf(answer, "%d", result);
        int fd_result = open(path, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
        write(fd_result, answer, strlen(answer));
        close(fd_result);
        kill(clientPid, SIGINT);
        exit(0);
    }
}

void alarmHandler(int sig) {
    printf("The server was closed because no service request was received for the last 60 seconds\n");
    exit(0);
}

int main() {
    signal(SIGINT, clientHandler);
    signal(SIGALRM, alarmHandler);
    alarm(60);
    while (1) {
        pause();
        alarm(0);
        alarm(60);
    }
    return 0;
}
