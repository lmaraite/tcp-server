#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <semaphore.h>
#include <fcntl.h>

#include "datenhaltung.h"
#include "../include/utils.h"
#include "configuration.h"

char *semPrefix = "tcpServer-keySem-";
int databaseCreated = 0;


//-----------------------------

char *readString(FILE* fileDescriptor){
	char tempChar;
	int stringLength = 1;
	char *inputString;
	inputString = malloc(stringLength * sizeof(char));
	while((tempChar = getc(fileDescriptor)) && (tempChar != EOF) && (tempChar != '\n')){
                inputString[stringLength-1] = tempChar;
              	stringLength++;
                inputString = realloc(inputString, stringLength*sizeof(char));
        };
	char *returnString;
	returnString = realloc(inputString,stringLength*sizeof(char));
	returnString[stringLength-1] = '\0';

	inputString = NULL;
	free(inputString);
	return returnString;
}

char *concatenate(char* string1, char* string2){
    int stringLength = strlen(string1) + strlen(string2) +1;
    char *combinedString;
    combinedString = malloc(stringLength);
    strcpy(combinedString, string1);
    strcat(combinedString, string2);
    return combinedString;
}

int createDatabase() {
    pid_t pid = fork();
    if(pid == 0) {
        return execl("/usr/bin/mkdir", "mkdir", "-p", config.PATH, NULL);
    } else {
        waitpid(pid, 0, 0);
        return 0;
    }
}

//-----------------------------

Result find_by_key(char* key){
    debug("find by key %s", key);
	sem_t *fileSem;
	char *fileSemName = concatenate(semPrefix,key);
	fileSem = sem_open(fileSemName, O_CREAT, 0644, 1);
	sem_wait(fileSem);

	FILE *keyFile;
	char *keyPath = concatenate(config.PATH,key);

	keyFile = fopen(keyPath,"r");
	free(keyPath);

	Result result;
	if(keyFile == NULL){
        debug("no entry found for key %s", key);
		result.error_code = 1002;
		sem_post(fileSem);
		return result;
	}

	char* value;
	value = readString(keyFile);

	fclose(keyFile);

	sem_post(fileSem);
	free(fileSemName);

	result.value = value;
	result.error_code = 0;
    debug("found value %s for key %s", result.value, key);
	return result;
}

int save(char* key, char* value){
     debug("save key %s with value %s", key, value);
     if(strcmp(key, "") == 0) {
        debug("cannot save empty key");
        return 1001;
     }
     if(!databaseCreated) {
         int c_return = createDatabase();
         if(c_return != 0) {
             error("could not create database");
             return c_return + 1000;
         }
         databaseCreated = 1;
     }
  
	  sem_t *fileSem;
	  char *fileSemName = concatenate(semPrefix,key);
	  fileSem = sem_open(fileSemName, O_CREAT, 0644, 1);

	  FILE *keyFile;
	  char *keyPath = concatenate(config.PATH,key);

	  sem_wait(fileSem);

	  keyFile = fopen(keyPath,"w");
      free(keyPath);

    if(keyFile == NULL) {
        debug("file %s could not be opened", keyFile);
        sem_post(fileSem);
        free(fileSemName);
        return 1001;
    }
    fprintf(keyFile, "%s", value);
    fclose(keyFile);

    sem_post(fileSem);
    free(fileSemName);
    debug("saved key %s with value %s", key, value);

    return 0;
}

int delete_by_key(char* key){
    debug("delete key %s", key);
	sem_t *fileSem;
	char *fileSemName = concatenate(semPrefix,key);
	fileSem = sem_open(fileSemName, O_CREAT, 0644, 1);

	sem_wait(fileSem);

	FILE *keyFile;
	char *keyPath = concatenate(config.PATH,key);
	keyFile = fopen(keyPath,"r");

	if(keyFile == NULL){
        debug("no entry found for key %s", key);
		sem_post(fileSem);
		return 1002;
	}
	fclose(keyFile);

	remove(keyPath);
	free(keyPath);
    debug("entry for key %s deleted", key);

	sem_post(fileSem);
	sem_unlink(fileSemName);
	free(fileSemName);

	return 0;
}
