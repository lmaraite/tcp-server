#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../include/utils.h"
#include "../include/datenhaltung.h"
#include "../include/applicationLayer.h"
#include "logger.h"

void notifyAll(char *key, char *message);
enum Position {keyPos, pidPos};
int op_and_save(char *systemCall, char *key);

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


Result op(char *systemCall, char *key) {
    Result result;
    result.error_code = op_and_save(systemCall, key);
    return result;
}

Result executeCommand(Command command) {

int beg(){
    sem_t *exclusiveSem;
    exclusiveSem = sem_open("tcpServer-exclusiveSem", O_CREAT, 0644, 1);
    sem_wait(exclusiveSem);

    FILE *exclusiveFile;

    exclusiveFile = fopen(".exclusive","r");

    if(exclusiveFile != NULL){
        int returnValue;
        int pid = getpid();
        int exclusiveID;

        fscanf(exclusiveFile,"%i",&exclusiveID);

        if (exclusiveID == pid){
            returnValue = 1003;
        }
        else {
            returnValue = 1002;
        }

        fclose(exclusiveFile);
        sem_post(exclusiveSem);
        return returnValue;
    }

    exclusiveFile = fopen(".exclusive","w");

    fprintf(exclusiveFile, "%d", getpid());
    fclose(exclusiveFile);
    sem_post(exclusiveSem);

    return 0;
}

int end (){
    sem_t *exclusiveSem;
    exclusiveSem = sem_open("tcpServer-exclusiveSem", O_CREAT, 0644, 1);
    sem_wait(exclusiveSem);

    FILE *exclusiveFile;
    exclusiveFile = fopen(".exclusive","r");

    if(exclusiveFile == NULL){
        sem_post(exclusiveSem);
        return 1002;
    }
    fclose(exclusiveFile);

    remove(".exclusive");
    sem_post(exclusiveSem);
    return 0;
}

int checkExclusive(){
  int pid = getpid();
  int exclusiveID;
  FILE *exclusiveFile;
  exclusiveFile = fopen(".exclusive", "r");

  if(exclusiveFile == NULL){
       return 0;
  }

  fscanf(exclusiveFile,"%i",&exclusiveID);

  if (exclusiveID == pid){
    return 0;
  }

  return 1;
}

Result executeCommand( Command command){

    Result result;
    char *formatedValue;

    int hasPermission = checkExclusive();
    if (hasPermission==1) {
        result.error_code = 1;
        result.value = calloc(sizeof(char), 34);
        strcpy(result.value, "Another user has exclusive access");
        return result;
    }
    debug("executing command %s", command.order);
    if(strcmp(command.order, "BEG")==0){
      int exclusiveStatus = beg();
      if(exclusiveStatus==0){
          result.value = calloc(sizeof(char), 30);
          strcpy(result.value, "You now have exclusive access");
      }
      else if(exclusiveStatus==1002){
          result.value = calloc(sizeof(char), 34);
          strcpy(result.value, "You already have exclusive access");
      }
      else if(exclusiveStatus==1003){
          result.error_code = 1;
          result.value = calloc(sizeof(char), 34);
          strcpy(result.value, "Another user has exclusive access");
          return result;
      }
      result.error_code = 0;
      return result;
    }

    if(strcmp(command.order, "END")==0){
      int exclusiveStatus = end();
      if(exclusiveStatus==0){
        result.value = calloc(sizeof(char), 34);
        strcpy(result.value, "You gave up your exclusive access");
        result.error_code = 0;
      }
      else if(exclusiveStatus==1002){
          result.value = calloc(sizeof(char), 28);
          strcpy(result.value, "Nobody has exclusive access");
      }
      result.error_code = 0;
      return result;
    }

    if(strcmp(command.order, "PUT")==0){
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
            free(result.value);
        }
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
    } else if (strcmp(command.order, "OP") == 0) {
        result = op(command.key, command.value);
        formatedValue = malloc(sizeof(char) * 15);
        sprintf(formatedValue, "%s", "exec successful");
        result.value = formatedValue;
    }
    else {
        result.error_code = 1;
        result.value = calloc(sizeof(char), 18);
        strcpy(result.value, "Command not found");
    }
    if (result.error_code == 0) {
        char *message = (char *) malloc(strlen(result.value) + strlen(command.key) + 4);
        sprintf(message, "> %s\n", result.value);
        if (strcmp(command.order, "GET") != 0) {
            notifyAll(command.key, message);
        }
    }
    return result;
}

void notifyAll(char *key, char *message) {
    struct sembuf up, down;
    down.sem_num = 0;
    down.sem_op = -1;
    down.sem_flg = SEM_UNDO;

    up.sem_num = 0;
    up.sem_op = 1;
    up.sem_flg = SEM_UNDO;

    int semId = semget(SUBSCRIPTION_SEM_KEY, 1, IPC_CREAT | 0644);
    semop(semId, &down, 1); // Enter critical are
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
    semop(semId, &up, 1); // Leave critical area
}


int op_and_save(char *systemCall, char *key) {
    char *sysCalls[] = {"date", "who", "uptime"};
    int sysCallsLength = 3;
    int fd[2];
    pipe(fd);

    if (fork() == 0) {
        dup2(fd[1], 1);
        close(fd[0]);

        for (int i = 0; i < sysCallsLength; i++) {
            if (strcmp(systemCall, sysCalls[i]) == 0) {
                execlp(systemCall, systemCall, 0);
            }
        }
        execlp("echo", "echo", "command not found", 0);
        return 1;
    } else {
        wait(0);
        dup2(fd[0], 0);
        save(key, readString(stdin));
        close(fd[0]);
        close(fd[1]);
    }
    return 0;
}

