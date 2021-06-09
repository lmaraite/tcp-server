#include "clientSession.h"

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

//TODO: remove
#include <stdio.h>

#include "utils.h"
#include "applicationLayer.h"

#define MAX_BUFFER_SIZE STRING_LENGTH * 3
#define ERROR_PREFIX "ERROR: "

int handleMessage(const int socketfd, char readBuffer[]);

static inline int isBiggerThanBuffer(int numBytesRead) {
    return numBytesRead > MAX_BUFFER_SIZE;
}

static inline int isNotEmpty(int numBytesRead) {
    return numBytesRead > 0;
}

int handleClient(const int socketfd) {
    char readBuffer[MAX_BUFFER_SIZE + 1];

    while(1) {
        int numBytesRead = recv(socketfd, readBuffer, MAX_BUFFER_SIZE +1, 0);
        if(isBiggerThanBuffer(numBytesRead)) {
            char dest_string[25];
            sprintf(dest_string, "%s%s\n", ERROR_PREFIX, "message too long");
            send(socketfd, dest_string, 24, 0);
        } else if(numBytesRead < 0) {
            return ANY_SOCKET_EXCEPTION;
        } else if(isNotEmpty(numBytesRead)) {
            readBuffer[numBytesRead] = '\0';
            int handleMessageReturn = handleMessage(socketfd, readBuffer);
            if(handleMessageReturn == ANY_SOCKET_EXCEPTION) {
                return ANY_SOCKET_EXCEPTION;
            }
            if(handleMessageReturn == -1) {
                return 0;
            }
        }
    }

    return 0;
}

int handleMessage(const int socketfd, char readBuffer[]) {
    Command command = parseStringToCommand(readBuffer);
    if(strcmp(command.order, "QUIT") == 0) {
        printf("Closing client session %d\n", socketfd);
        return close(socketfd) < 0 ? ANY_SOCKET_EXCEPTION : -1;
    }
    Result result = executeCommand(command);
    int sendResult = -1;
    char* answerToClient = NULL;
    if( result.error_code != 0) {
        answerToClient = (char*) malloc(strlen(ERROR_PREFIX) + strlen(result.value) + 1);
        sprintf(answerToClient, "%s%s\n", ERROR_PREFIX, result.value);
    } else {
        answerToClient = malloc(sizeof(result.value) + 3);
        sprintf(answerToClient, "%s\n", result.value);
    }
    if(send(socketfd, answerToClient, strlen(answerToClient) +1, 0) < 0) {
        return ANY_SOCKET_EXCEPTION;
    }
    free(answerToClient);
    free(result.value);
    return 0;
}
