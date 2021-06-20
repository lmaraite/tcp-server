#include "clientSession.h"

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <stdio.h>
#include <signal.h>
#include "../include/utils.h"
#include "../include/applicationLayer.h"

#define MAX_BUFFER_SIZE STRING_LENGTH * 3
#define ERROR_PREFIX "ERROR: "

int handleMessage(const int socketfd, char readBuffer[]);
void cleanUp();
int subscriptionReceiver(const int socketfd);

static inline int isBiggerThanBuffer(int numBytesRead) {
    return numBytesRead > MAX_BUFFER_SIZE;
}

static inline int isNotEmpty(int numBytesRead) {
    return numBytesRead > 0;
}

int shmId, receiverPid;
char *subscriptions;

int handleClient(const int socketfd) {
    char readBuffer[MAX_BUFFER_SIZE + 1];
    shmId = shmget(SUBSCRIPTION_SHM_KEY, BUFSIZ, IPC_CREAT | 0644);
    subscriptions = (char *)shmat(shmId, 0, 0);

    receiverPid = subscriptionReceiver(socketfd);

    while(1) {
        int numBytesRead = recv(socketfd, readBuffer, MAX_BUFFER_SIZE +1, 0);
        if(isBiggerThanBuffer(numBytesRead)) {
            char dest_string[27];
            sprintf(dest_string, "> %s%s\n", ERROR_PREFIX, "message too long");
            send(socketfd, dest_string, 24, 0);
        } else if(numBytesRead < 0) {
            cleanUp();
            return ANY_SOCKET_EXCEPTION;
        } else if(isNotEmpty(numBytesRead)) {
            readBuffer[numBytesRead] = '\0';
            int handleMessageReturn = handleMessage(socketfd, readBuffer);
            if(handleMessageReturn == ANY_SOCKET_EXCEPTION) {
                cleanUp();
                return ANY_SOCKET_EXCEPTION;
            }
            if(handleMessageReturn == -1) {
                cleanUp();
                return 0;
            }
        }
    }
}

int handleMessage(const int socketfd, char readBuffer[]) {
    Command command = parseStringToCommand(readBuffer);
    if(strcmp(command.order, "QUIT") == 0) {
        printf("INFO: closing client session %d\n", socketfd);
        return close(socketfd) < 0 ? ANY_SOCKET_EXCEPTION : -1;
    }
    char* answerToClient = NULL;
    if (strcmp(command.order, "SUB") == 0) {
        sprintf(subscriptions, "%s%s$%d#", subscriptions, command.key, receiverPid);
        answerToClient = (char *) malloc(strlen(command.key) + 8);
        sprintf(answerToClient, "> SUB:%s\n", command.key);
        printf("%s\n", subscriptions);
        goto send;
    }

    Result result = executeCommand(command);
    if( result.error_code != 0) {
        answerToClient = (char*) malloc(strlen(ERROR_PREFIX) + strlen(result.value) + 4);
        sprintf(answerToClient, "> %s%s\n", ERROR_PREFIX, result.value);
    } else {
        answerToClient = malloc(strlen(result.value) + 4);
        sprintf(answerToClient, "> %s\n", result.value);
    }
    send:
    if(send(socketfd, answerToClient, strlen(answerToClient) +1, 0) < 0) {
        return ANY_SOCKET_EXCEPTION;
    }
    free(answerToClient);
    free(result.value);
    return 0;
}

void cleanUp() {
    kill(receiverPid, SIGKILL);
}

int subscriptionReceiver(const int socketfd) {
    int pid = fork();
    if (pid == 0) {
        int id = msgget(MSG_QUEUE_KEY, IPC_CREAT | 0644);
        while(1) {
            struct message msg;
            msgrcv(id, &msg, BUFSIZ, getpid(), 0);
            send(socketfd, msg.text, strlen(msg.text), 0);
        }
    }
    return pid;
}