#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

Result executeCommand( Command command){
    Result result;
    char *formatedValue;
    if(strcmp(command.order, "PUT")==0){
        result = put(command.key, command.value);
        formatedValue = malloc(sizeof(command.key) + sizeof(command.value) + (sizeof(char) * 6));
        sprintf(formatedValue, "%s:%s:%s", "PUT", command.key, command.value);
        result.value = formatedValue;
        return result;
    }
    if(strcmp(command.order, "GET")==0){
        result = find_by_key(command.key);
        if (result.error_code == 1002) {
            formatedValue = malloc(sizeof(command.key) + sizeof("key_nonexistent") + (sizeof(char) * 6));
            sprintf(formatedValue, "%s:%s:%s", "GET", command.key, "key_nonexistent");
        }
        else {
            formatedValue = malloc(sizeof(command.key) + sizeof(command.value) + (sizeof(char) * 6));
            sprintf(formatedValue, "%s:%s:%s", "GET", command.key, result.value);
        }
        result.value = formatedValue;
        return result;
    }
    if(strcmp(command.order, "DEL")==0){
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
        return result;
    }
    else{
        result.error_code = 1;
        result.value = "Command not found\n";
        return result;
    }
}
