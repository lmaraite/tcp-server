#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <semaphore.h>
#include <fcntl.h>

#include "datenhaltung.h"
#include "../include/utils.h"

char *storage = "storage/";
char *semPrefix = "tcpServer-keySem-";

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
    int stringLength = strlen(string1) + strlen(string2);
    char *combinedString;
    combinedString = malloc(stringLength);
    strcpy(combinedString, string1);
    strcat(combinedString, string2);
    return combinedString;
}

//-----------------------------

Result find_by_key(char* key){
	sem_t *fileSem;
	char *fileSemName = concatenate(semPrefix,key);
	fileSem = sem_open(fileSemName, O_CREAT, 0644, 1);
	sem_wait(fileSem);

	FILE *keyFile;
	char *keyPath = concatenate(storage,key);

	keyFile = fopen(keyPath,"r");
	free(keyPath);

	Result result;
	if(keyFile == NULL){
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

	return result;
}

int save(char* key, char* value){

	sem_t *fileSem;
	char *fileSemName = concatenate(semPrefix,key);
	fileSem = sem_open(fileSemName, O_CREAT, 0644, 1);

	FILE *keyFile;
	char *keyPath = concatenate(storage,key);

	sem_wait(fileSem);

	keyFile = fopen(keyPath,"w");
	free(keyPath);

	fprintf(keyFile, "%s", value);
	fclose(keyFile);

	sem_post(fileSem);
	free(fileSemName);

	return 0;
}

int delete_by_key(char* key){
	sem_t *fileSem;
	char *fileSemName = concatenate(semPrefix,key);
	fileSem = sem_open(fileSemName, O_CREAT, 0644, 1);

	sem_wait(fileSem);

	FILE *keyFile;
	char *keyPath = concatenate(storage,key);
	keyFile = fopen(keyPath,"r");

	if(keyFile == NULL){
		sem_post(fileSem);
		return 1002;
	}
	fclose(keyFile);

	remove(keyPath);
	free(keyPath);

	sem_post(fileSem);
	sem_unlink(fileSemName);
	free(fileSemName);

	return 0;
}
