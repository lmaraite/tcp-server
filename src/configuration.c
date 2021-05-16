#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/configuration.h"
/*
    Gibt SUCCESS oder eine Fehlermeldung über 1000 zurück
*/
enum Position {key, value};

const char PORT_KEY[] = "PORT";
const char PATH_KEY[] = "PATH";
const char MAX_SESSIONS_KEY[] = "MAX_SESSIONS";

Configuration config = {5678, "./data", 5};

int loadConfig(char path[]) {
    char c;
    FILE *fp;
    fp = fopen(path, "r");
    if (fp == NULL) {
        printf("An error occurred while opening the config file. The application will stick to its default config.\n");
        return 1001;
    }
    char keyContent[128] = "";
    char valueContent[128] = "";
    enum Position position = key;
    while ((c = fgetc(fp)) != EOF) {
        if (c == '=') {
            position = value;
        } else if (c == '\n') {
            if (strcmp(keyContent, PORT_KEY) == 0) {
                config.PORT = atoi(valueContent);
            }
            if (strcmp(keyContent, PATH_KEY) == 0) {
                strcpy(config.PATH, valueContent);
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

