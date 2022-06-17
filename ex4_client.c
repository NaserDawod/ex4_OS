#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>


void sigHandler(int sig) {
    int pid = getpid();
    char clientPid[10], clientFile[40];
    sprintf(clientPid, "%d", pid);
    strcpy(clientFile, "to_client_");
    strcat(clientFile, clientPid);
    int fd = open(clientFile, 'r');
    char answer[100], c;
    int i;
    for (i = 0; read(fd, &c, 1) > 0; ++i) {
        answer[i] = c;
    }
    answer[i-1] = 0;
    close(fd);
    remove(clientFile);
    printf("%s\n", answer);
}

void problemHandler(int sig) {
    printf("CANNOT_DIVIDE_BY_ZERO\n");
    exit(0);
}

void alarmHandler2(int sig) {
    printf("Client closed because no response was received from the server for 30 seconds\n");
    exit(0);
}

void sendFile(int fd, char** argv, char* clientPid) {
    write(fd, clientPid, strlen(clientPid));
    write(fd, " ", 1);
    write(fd, argv[2], strlen(argv[2]));
    write(fd, " ", 1);
    write(fd, argv[3], strlen(argv[3]));
    write(fd, " ", 1);
    write(fd, argv[4], strlen(argv[4]));
}

int main(int argc, char** argv) {
    if (argc < 5) {
        printf("ERROR_FROM_EX4\n");
        return 0;
    }
    signal(SIGINT, sigHandler);
    signal(SIGALRM, alarmHandler2);
    signal(SIGILL, problemHandler);
    int pid = getpid(), waiting;
    char clientPid[10];
    sprintf(clientPid, "%d", pid);
    int serverPid = atoi(argv[1]);
    alarm(30);
    for (int i = 0; i < 10; ++i) {
        int fd = open("to_srv", O_CREAT | O_EXCL | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
        if (fd < 0) {
            waiting = (rand() % 5) + 1;
            sleep(waiting);
            continue;
        }
        alarm(0);
        sendFile(fd, argv, clientPid);
        close(fd);
        kill(serverPid, SIGINT);
        pause();
        return 0;
    }
    alarmHandler2(0);
    return 0;
}