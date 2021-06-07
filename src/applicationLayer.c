#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/utils.h"
#include "../include/datenhaltung.h"
#include "../include/applicationLayer.h"

Result put(char* key, char* value ){
    Result result;
    if(save(key,value)==0){
        result.value = value;
        result.error_code = 0;
        return result;
    }
    else{
        result.error_code = save(key, value);
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
    if(strcmp(command.order, "put")==0){
        return put(command.key, command.value);
    }
    if(strcmp(command.order, "get")==0){
        return find_by_key(command.key);
    }
    if(strcmp(command.order, "del")==0){
        return del(command.key);
    }
    else{
        result.error_code = 1;
        return result;
    }
}
