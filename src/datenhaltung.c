#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "datenhaltung.h"
#include "../include/utils.h"
#include "configuration.h"

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
    strcat(combinedString, string1);
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
     FILE *keyFile;
     char *keyPath = concatenate(config.PATH, key);

	 keyFile = fopen(keyPath,"r");
	 free(keyPath);
	 Result result;
     if(keyFile == NULL){
          result.value = NULL;
          result.error_code = 1002;
          return result;
     }

     char* value;
     value = readString(keyFile);

     fclose(keyFile);

     result.value = value;
     result.error_code = 0;

     return result;
}

int save(char* key, char* value){
     if(!databaseCreated) {
         int c_return = createDatabase();
         if(c_return != 0) {
             return c_return + 1000;
         }
         databaseCreated = 1;
     }
     FILE *keyFile;
     char *keyPath = concatenate(config.PATH, key);
     keyFile = fopen(keyPath,"w");
     free(keyPath);

     fprintf(keyFile, "%s", value);
     fclose(keyFile);

     return 0;
}

int delete_by_key(char* key){
  FILE *keyFile;
  char *keyPath = concatenate(config.PATH,key);
  keyFile = fopen(keyPath,"r");

  if(keyFile == NULL){
       return 1002;
  }
  fclose(keyFile);

  remove(keyPath);
  free(keyPath);
  return 0;
}
