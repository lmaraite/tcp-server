#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <netinet/in.h>
#include <string.h>

#include "../include/utils.h"
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
    info("server running on port: %d", socketPort);

    //Init subscription list no critical area because at this point this is the only process
    int shmId = shmget(SUBSCRIPTION_SHM_KEY, BUFSIZ, IPC_CREAT | 0644);
    char *subscriptions = shmat(shmId, 0, 0);
    strncpy(subscriptions, "", 1);

    //Init semaphore for subscriptions
    int semId = semget(SUBSCRIPTION_SEM_KEY, 1, IPC_CREAT | 0644);
    unsigned short signals[1];
    signals[0] = 1;
    semctl(semId, 0, SETALL, signals);


    while (1) {
        clientSocket = accept(serverSocket,
                              (struct sockaddr *) &clientAddress,
                              &clientAddress_len);
        if (fork() == 0) {
            char *connected = "\nWelcome to the key value storage.\n";
            info("client connected on socket %d", clientSocket);
            send(clientSocket, connected, strlen(connected), 0);
            int errorCode = handleClient(clientSocket);
            info("closed socket %d with error code: %d", clientSocket, errorCode);
            break;
        } else {
            close(clientSocket);
        }
    }

}
