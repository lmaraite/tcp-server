#ifndef TCP_SERVER_CONFIGURATION_H
#define TCP_SERVER_CONFIGURATION_H

 typedef struct {
    int PORT;
    char PATH[512];
    int MAX_SESSIONS;
} Configuration ;

static Configuration config = {5678, "./data", 5};

/*
    Gibt SUCCESS oder eine Fehlermeldung über 1000 zurück
*/
int loadConfig(char path[]);

#endif //TCP_SERVER_CONFIGURATION_H
