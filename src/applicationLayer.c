#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include "../include/utils.h"
#include "../include/datenhaltung.h"
#include "../include/applicationLayer.h"

void notifyAll(char *key, char *message);
enum Position {keyPos, pidPos};

Result put(char *key, char *value) {
    Result result;
    int status = save(key, value);
    if (status == 0) {
        result.value = value;
        result.error_code = status;
        return result;
    } else {
        result.error_code = status;
        return result;
    }
}

Result del(char *key) {
    Result result;
    result.error_code = delete_by_key(key);
    return result;
}

Result executeCommand(Command command) {
    Result result;
    char *formatedValue;
    if (strcmp(command.order, "PUT") == 0) {
        result = put(command.key, command.value);
        formatedValue = malloc(sizeof(command.key) + sizeof(command.value) + (sizeof(char) * 6));
        sprintf(formatedValue, "%s:%s:%s", "PUT", command.key, command.value);
        result.value = formatedValue;
    } else if (strcmp(command.order, "GET") == 0) {
        result = find_by_key(command.key);
        if (result.error_code == 1002) {
            formatedValue = malloc(sizeof(command.key) + sizeof("key_nonexistent") + (sizeof(char) * 6));
            sprintf(formatedValue, "%s:%s:%s", "GET", command.key, "key_nonexistent");
        } else {
            formatedValue = malloc(sizeof(command.key) + sizeof(command.value) + (sizeof(char) * 6));
            sprintf(formatedValue, "%s:%s:%s", "GET", command.key, result.value);
        }
        free(result.value);
        result.value = formatedValue;
    } else if (strcmp(command.order, "DEL") == 0) {
        result = del(command.key);
        if (result.error_code == 1002) {
            formatedValue = malloc(sizeof(command.key) + sizeof("key_nonexistent") + (sizeof(char) * 6));
            sprintf(formatedValue, "%s:%s:%s", "DEL", command.key, "key_nonexistent");
        } else {
            formatedValue = malloc(sizeof(command.key) + sizeof("key_deleted") + (sizeof(char) * 5));
            sprintf(formatedValue, "%s:%s:%s", "DEL", command.key, "key_deleted");
        }
        result.value = formatedValue;
    } else {
        result.error_code = 1;
        result.value = calloc(sizeof(char), 18);
        strcpy(result.value, "Command not found");
    }
    if (result.error_code == 0) {
        char *message = (char *) malloc(strlen(result.value) + strlen(command.key) + 4);
        sprintf(message, "> %s\n", result.value);
        notifyAll(command.key, message);
    }
    return result;
}

void notifyAll(char *key, char *message) {
    int shmId = shmget(SUBSCRIPTION_SHM_KEY, BUFSIZ, IPC_CREAT | 0644);
    char *subscriptions = shmat(shmId, 0, 0);
    int id = msgget(MSG_QUEUE_KEY, IPC_CREAT | 0644);
    struct message msg;
    msg.type = 1;
    strncpy(msg.text, message, strlen(message));

    int i = 0;
    char currentKey[STRING_LENGTH] = "";
    char pid[6] = "";
    enum Position position = keyPos;
    while (subscriptions[i] != '\0') {
        char c = subscriptions[i];
        printf("DEBUG: currentKey = %s | pid = %s | c = %c\n", currentKey, pid, c);
        if (c == '#') {
            if (strcmp(key, currentKey) == 0) {
                msg.type = atoi(pid);
                msgsnd(id, &msg, strlen(message), 0);
            }
            position = keyPos;
            strcpy(currentKey, "");
            strcpy(pid, "");
        }
        else if (c == '$') {
            position = pidPos;
        } else {
            if (position == keyPos) {
                strncat(currentKey, &c, 1);
            }
            if (position == pidPos) {
                strncat(pid, &c, 1);
            }
        }
        i++;
    }
}
