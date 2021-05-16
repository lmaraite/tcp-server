#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "datenhaltung.h"
struct Result executeCommand(struct Command command){
    struct Result result;
if(comman.order == "put"){
   return put(command.key, command.value);
}
else if(comm.order == "get"){
    return get(command.key);
}
else if(comm.order == "del"){
    return del(key);
}
else if(comm.order == "quit"){
    return quit();
}
else{
    result.error_code = 1;
    return result;
}
};

struct Result put(char* key, char value ){
     result.key = key;
     result.value = value;
     result.error_code = save(key, value);
     return result;

};
struct Result get(char* key){
    return get_by_key(key);

};
struct Result del(char* key){
    result.key = key;
    result.error_code = delete(key);
    return result;

};
void quit(){

};
