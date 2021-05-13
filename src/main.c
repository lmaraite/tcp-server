#include <stdio.h>
#include "greeter.h"
#include "configuration.h"

int main() {
    printf("%s\n", greet("world"));
    loadConfig("config");
    return 0;
}

