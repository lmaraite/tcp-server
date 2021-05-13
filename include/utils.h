//
// Created by leon on 12.05.21.
//

#ifndef TCP_SERVER_UTILS_H
#define TCP_SERVER_UTILS_H

#define SUCCESS 0;
#define COMMAND_NOT_FOUND 1;
#define KEY_NOT_FOUND 2;
#define MISSING_ARGUMENT 3;
#define TO_MANY_ARGUMENTS 4;
#define ANY_SOCKET_EXCEPTION 5;

typedef struct {
    char order[128];
    char key[128];
    char value[128];
} Command;

typedef struct {
    char *value;
    int error_code;
} Result;

/*
    Trennt commandAsString an den Leerzeichen und speichert die ersten drei
    resultierenden Strings in Command.order, Command.key und Command.value
*/
Command parseStringToCommand(char *commandAsString);

#endif //TCP_SERVER_UTILS_H
