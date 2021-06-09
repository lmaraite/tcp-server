#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#include "../include/server.h"
#include "../include/clientSession.h"
#include "../include/configuration.h"


/*
     Startet einen Socket-Server auf Port 7777 und wartet auf eingehende TCP-Verbindungen.
     Anschließend wird für jede angenommene Verbindung ein Child-Process erstellt
*/
void runServer() {

    loadConfig("./config");

    int socketPort = config.PORT;                 // Port
    int maxClients = config.MAX_SESSIONS;        // max. Client Sessions

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serverAddress;
    int serverAddress_len = sizeof(serverAddress);
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;    // INADDR_ANY: localhost
    serverAddress.sin_port = htons(socketPort); // htons: int in network-byte-order

    bind(serverSocket,
         (struct sockaddr *) &serverAddress,
         serverAddress_len);

    struct sockaddr_in clientAddress;
    int clientSocket;
    int clientAddress_len = sizeof(clientAddress);

    listen(serverSocket, maxClients);
    printf("Server running on port: %d\n", socketPort);
    while (1) {
        clientSocket = accept(serverSocket,
                              (struct sockaddr *) &clientAddress,
                              &clientAddress_len);
        if (fork() == 0) {
            char *connected = "Connected.\n";
            send(clientSocket, connected, strlen(connected), 0);
            int errorCode = handleClient(clientSocket);
            printf("Closed socket with error code: %d\n", errorCode);
            break;
        } else {
            close(clientSocket);
        }
    }

}