#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#include "../include/utils.h"


struct Result{
    char *value;
    int error_code;
};

struct Result result;

char *storage = "storage/";

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
    int stringLength = sizeof(string1) + sizeof(string2);
    char *combinedString;
    combinedString = malloc(stringLength);
    strcat(combinedString, string1);
    strcat(combinedString, string2);
    return combinedString;
}

//-----------------------------

struct Result find_by_key(char* key){
     FILE *keyFile;
     char *keyPath = concatenate(storage,key);
     printf("%s\n", keyPath);
     keyFile = fopen(keyPath,"r");
     free(keyPath);

     if(keyFile == NULL){
          result.error_code = 1002;
          return result;
     }

     char* value;
     value = readString(keyFile);

     fclose(keyFile);

     result.value = value;
     result.error_code = 0;

     free(value);
     return result;
}

int save(char* key, char* value){
     FILE *keyFile;
     char *keyPath = concatenate(storage,key);
     printf("%s\n", keyPath);
     keyFile = fopen(keyPath,"w");
     free(keyPath);

     fprintf(keyFile, "%s", value);
     fclose(keyFile);

     return 0;
}

int delete(char* key){
  FILE *keyFile;
  char *keyPath = concatenate(storage,key);
  printf("%s\n", keyPath);
  keyFile = fopen(keyPath,"r");

  if(keyFile == NULL){
       return 1002;
  }
  fclose(keyFile);

  remove(keyPath);
  free(keyPath);
  return 0;

}

int main(int argc, char const *argv[]) {
     printf("\n");

     result = find_by_key("saveFile");
     printf("Value: %s\n",result.value);
     if(result.error_code==0){
       printf("SUCCESS\n");
     } else{
       printf("Error: %i\n",result.error_code);
     }
     printf("\n");

     int deleteError = delete("saveFile2");
     printf("deleteError: %i\n",deleteError);
     printf("\n");

     int saveError = save("saveFile", "saved_key");
     printf("saveError: %i\n", saveError);
     printf("\n");

     return 0;
}
