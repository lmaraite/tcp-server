#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

#include "../include/utils.h"
#include "../include/datenhaltung.h"
#include "../include/applicationLayer.h"

Result put(char* key, char* value ){
    Result result;
    int status = save(key,value);
    if(status == 0){
        result.value = value;
        result.error_code = status;
        return result;
    }
    else{
        result.error_code = status;
        return result;
    }
}

Result del(char* key){
    Result result;
    result.error_code = delete_by_key(key);
    return result;
}

int beg(){
    sem_t *exclusiveSem;
    exclusiveSem = sem_open("tcpServer-exclusiveSem", O_CREAT, 0644, 1);
    sem_wait(exclusiveSem);

    FILE *exclusiveFile;

    exclusiveFile = fopen(".exclusive","r");

    if(exclusiveFile != NULL){
        fclose(exclusiveFile);
        sem_post(exclusiveSem);
        return 1002;
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
        result.value="Another user has exclusive permissions";
        result.malloced=1;
        return result;
    }

    if(strcmp(command.order, "BEG")==0){
      int exclusiveStatus = beg();
      if(exclusiveStatus==0){
          result.value = "You now have exclusive permissions";
      }
      else if(exclusiveStatus==1002){
          result.value= "You already have exclusive permissions";
      }
      result.error_code = 0;
      result.malloced=1;
      return result;
    }

    if(strcmp(command.order, "END")==0){
      int exclusiveStatus = end();
      if(exclusiveStatus==0){
        result.value = "You gave up your exclusive permissions";
        result.error_code = 0;
      }
      else if(exclusiveStatus==1002){
          result.value="Nobody has exclusive permissions";
      }
      result.error_code = 0;
      result.malloced=1;
      return result;
    }

    if(strcmp(command.order, "PUT")==0){
        result = put(command.key, command.value);
        formatedValue = malloc(sizeof(command.key) + sizeof(command.value) + (sizeof(char) * 6));
        sprintf(formatedValue, "%s:%s:%s", "PUT", command.key, command.value);
        result.value = formatedValue;
    } else if(strcmp(command.order, "GET")==0){
        result = find_by_key(command.key);
        if (result.error_code == 1002) {
            formatedValue = malloc(sizeof(command.key) + sizeof("key_nonexistent") + (sizeof(char) * 6));
            sprintf(formatedValue, "%s:%s:%s", "GET", command.key, "key_nonexistent");
        }
        else {
            formatedValue = malloc(sizeof(command.key) + sizeof(command.value) + (sizeof(char) * 6));
            sprintf(formatedValue, "%s:%s:%s", "GET", command.key, result.value);
        }
        free(result.value);
        result.value = formatedValue;
    } else if(strcmp(command.order, "DEL")==0){
        result =  del(command.key);
        if (result.error_code == 1002) {
            formatedValue = malloc(sizeof(command.key) + sizeof("key_nonexistent") + (sizeof(char) * 6));
            sprintf(formatedValue, "%s:%s:%s", "DEL", command.key, "key_nonexistent");
        }
        else {
            formatedValue = malloc(sizeof(command.key) + sizeof("key_deleted") + (sizeof(char) * 5));
            sprintf(formatedValue, "%s:%s:%s", "DEL", command.key, "key_deleted");
        }
        result.value = formatedValue;
    } else{
        result.error_code = 1;
        result.value = calloc(sizeof(char), 18);
        strcpy(result.value, "Command not found");
    }
    return result;
}
