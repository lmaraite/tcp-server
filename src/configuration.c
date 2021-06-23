#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/configuration.h"
#include "../include/utils.h"
/*
    Gibt SUCCESS oder eine Fehlermeldung über 1000 zurück
*/
enum Position {key, value};

const char PORT_KEY[] = "PORT";
const char PATH_KEY[] = "PATH";
const char MAX_SESSIONS_KEY[] = "MAX_SESSIONS";

Configuration config = {5678, "./data", 5};

int setPath(char path[]);

int loadConfig(char configPath[]) {
    char c;
    FILE *fp;
    fp = fopen(configPath, "r");
    if (fp == NULL) {
        printf("An error occurred while opening the config file. The application will stick to its default config.\n");
        return 1001;
    }
    char keyContent[STRING_LENGTH] = "";
    char valueContent[STRING_LENGTH] = "";
    enum Position position = key;
    while ((c = fgetc(fp)) != EOF) {
        if (c == '=') {
            position = value;
        } else if (c == '\n') {
            if (strcmp(keyContent, PORT_KEY) == 0) {
                config.PORT = atoi(valueContent);
            }
            if (strcmp(keyContent, PATH_KEY) == 0) {
                if(setPath(valueContent) != 0) {
                    return 1002;
                };
            }
            if (strcmp(keyContent, MAX_SESSIONS_KEY) == 0) {
                config.MAX_SESSIONS = atoi(valueContent);
            }
            strcpy(keyContent, "");
            strcpy(valueContent, "");
            position = key;
        } else {
            if (position == key) {
                strncat(keyContent, &c, 1);
            }
            if (position == value) {
                strncat(valueContent, &c, 1);
            }
        }
    }
    fclose(fp);
    return 0;
}

int setPath(char path[]) {
    strcpy(config.PATH, path);
    int pathLen = strlen(config.PATH);
    if(config.PATH[pathLen -1] != '/') {
        if(pathLen +2 > PATH_MAX) {
            return 1;
        }
        config.PATH[pathLen] = '/';
        config.PATH[pathLen +1] = '\0';
    }
    return 0;
}
