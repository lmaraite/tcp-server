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
const char LOGGING_LEVEL_KEY[] = "LOGGING_LEVEL";

Configuration config = {5678, "./data", 5};

int setPath(char path[]);
LoggingLevel parseLoggingLevel(char levelStr[]);

int loadConfig(char configPath[]) {
    char c;
    FILE *fp;
    info("read config at path %s", configPath);
    fp = fopen(configPath, "r");
    if (fp == NULL) {
        error("An error occurred while opening the config file. The application will stick to its default config.\n");
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
            if (strcmp(keyContent, LOGGING_LEVEL_KEY) == 0) {
                LoggingLevel loggingLevel;
                if((loggingLevel = parseLoggingLevel(valueContent)) < 0) {
                    return 1003;
                }
                config.LOGGING_LEVEL = loggingLevel;
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
    info("config.PORT set to %d", config.PORT);
    info("config.PATH_KEY set to %s", config.PATH);
    info("config.MAX_SESSIONS set to %d", config.MAX_SESSIONS);
    info("config.LOGGING_LEVEL set to %d", config.LOGGING_LEVEL);
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

LoggingLevel parseLoggingLevel(char loggingLevelStr[]) {
    if(strcmp("off", loggingLevelStr) == 0) {
        return OFF;
    }
    if(strcmp("error", loggingLevelStr) == 0) {
        return ERROR;
    }
    if(strcmp("warn", loggingLevelStr) == 0) {
        return WARN;
    }
    if(strcmp("info", loggingLevelStr) == 0) {
        return INFO;
    }
    if(strcmp("debug", loggingLevelStr) == 0) {
        return DEBUG;
    }
    if(strcmp("trace", loggingLevelStr) == 0) {
        return TRACE;
    }
    return -1;
}
