#ifndef TCP_SERVER_CONFIGURATION_H
#define TCP_SERVER_CONFIGURATION_H

 typedef struct {
    int PORT;
    char PATH[512];
    int MAX_SESSIONS;
} Configuration ;

extern Configuration config;

/*
    Gibt SUCCESS oder eine Fehlermeldung über 1000 zurück.
    Lädt die Config aus der in path angegebenen Datei.
*/
int loadConfig(char path[]);

#endif //TCP_SERVER_CONFIGURATION_H
