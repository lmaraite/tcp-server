#include <stdio.h>
#include "greeter.h"
#include "configuration.h"
#include "../include/greeter.h"
#include "../include/configuration.h"

int main() {
    printf("%s\n", greet("world"));
    loadConfig("config");
    printf("PORT: %d\n", config.PORT);
    printf("PATH: %s\n", config.PATH);
    printf("MAX_SESSIONS: %d\n", config.MAX_SESSIONS);
    return 0;
}

