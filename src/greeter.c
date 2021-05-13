#include <stdio.h>
#include <stdlib.h>

char* greet(char *name) {
    char *greeting = malloc(128);
    sprintf(greeting, "Hello, %s!", name);
    return greeting;
}