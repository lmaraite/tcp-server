#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/utils.h"


struct Result result;

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

//-----------------------------

struct Result find_by_key(char* key){
     FILE *keyFile;
     keyFile = fopen(key,"r");

     if(keyFile == NULL){
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
     FILE *keyFile;
     keyFile = fopen(key,"w");
     fprintf(keyFile, "%s", value);
     fclose(keyFile);

     return 0;
}

int delete_by_key(char* key){
  FILE *keyFile;
  keyFile = fopen(key,"r");

  if(keyFile == NULL){
       return 1002;
  }
  fclose(keyFile);

  remove(key);
  return 0;

}
