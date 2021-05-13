#include <string.h>
#include "../include/utils.h"

Command parseStringToCommand(char commandAsString[]) {
    char str[strlen(commandAsString)];
    strcpy(str, commandAsString);
    char *delimiter = " ";
    char *token;
    Command command = {"", "", ""};
    token = strtok(str, delimiter);
    int i = 0;
    while (token) {
        if (i == 0) {
            strcpy(command.order, token);
            i++;
        } else if (i == 1) {
            strcpy(command.key, token);
            i++;
        } else {
            strcpy(command.value, token);
            break;
        }
        token = strtok(NULL, delimiter);
    }
    return command;
}
