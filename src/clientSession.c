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
            char dest_string[24];
            strcpy(dest_string, ERROR_PREFIX);
            strcat(dest_string, "message too long");
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
    if( result.error_code != 0) {
        char* dest_string = (char*) malloc(strlen(ERROR_PREFIX) + strlen(result.value) + 1);
        strcpy(dest_string, ERROR_PREFIX);
        strcat(dest_string, result.value);
        sendResult = send(socketfd, dest_string, strlen(dest_string) +1, 0);
        free(dest_string);
    } else {
        char *answereToClient = malloc(sizeof(result.value) + 3);
        sprintf(answereToClient, "%s\n", result.value);
        sendResult = send(socketfd, answereToClient, strlen(answereToClient) +1, 0);
        free(answereToClient);
    }
    if(sendResult < 0) {
        return ANY_SOCKET_EXCEPTION;
    }
    return 0;
}
