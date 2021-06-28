#include <string.h>
#include "../include/utils.h"
#include <stdio.h>

char *removeTrailingLineBreak(char *s) {
    if(s[strlen(s)-1] == '\n' && s[strlen(s)-2] == '\r') {
        s[strlen(s)-1] = '\0';
        s[strlen(s)-1] = '\0';
    }
    return s;
}

Command parseStringToCommand(char commandAsString[]) {
    char str[strlen(commandAsString)];
    char *strWithoutLineBreak = removeTrailingLineBreak(commandAsString);
    strcpy(str, strWithoutLineBreak);
    char *rest = str;
    char *delimiter = " ";
    char *token;
    Command command = {"", "", ""};
    token = strtok_r(str, delimiter, &rest);
    int i = 0;
    while (token) {
        if (i == 0) {
            strncpy(command.order, token, STRING_LENGTH);
            i++;
        } else if (i == 1) {
            strncpy(command.key, token, STRING_LENGTH);
            i++;
        } else {
            break;
        }
        token = strtok_r(NULL, delimiter, &rest);
    }
    return command;
}
