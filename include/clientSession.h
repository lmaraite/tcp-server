#ifndef CLIENT_SESSION_HEADER
#define CLIENT_SESSION_HEADER

/*
 * handleClient handles a client by parsing it's input, 
 * deligating the work to the application layer and writing
 * responses to the client socket.
 *
 * When the client session was successfully quit, 
 * SUCCESS is returned.
 * When an error regarding the socket communication occurs,
 * errno is set as defined by send and ANY_SOCKET_EXCEPTION 
 * is returned.
 *
 * Params:
 * 	socketfd: client socket as returned by accept
 * Returns:
 * 	SUCCESS or ANY_SOCKET_EXCEPTION
 * */
int handleClient(int socketfd);

#endif
