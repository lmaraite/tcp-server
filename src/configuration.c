#include <stdio.h>
#include "../include/configuration.h"
/*
    Gibt SUCCESS oder eine Fehlermeldung über 1000 zurück
*/
int loadConfig(char path[]) {
    printf("----- Loading config -----\n");
    char c;
    FILE *fp;
    fp = fopen(path, "r");
    if (fp == NULL) {
        printf("Error while opening the config file.\n");
        return 1001;
    }
    while ((c = fgetc(fp)) != EOF)
        printf("%c", c);
    fclose(fp);
    return 0;
}

