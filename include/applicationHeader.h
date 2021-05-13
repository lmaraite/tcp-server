#ifndef APPLICATIONHEADER_H_INCLUDED
#define APPLICATIONHEADER_H_INCLUDED

//Entscheidet ob und welcher Befehl ausgef�hrt wird, gibt Result an ClientSession zur�ck. M�gliche Error: COMMAND_NOT_FOUND, MISSING_ARGUMENT, TO_MANY_ARGUMENTS
struct Result executeCommand(struct Command command);

//Gibt Key und Value an Datenhaltung zur speicherung weiter, gibt Result an ClientSession zur�ck.
struct Result Put(struct Command comm);

//Fragt den Value zum gegebenen Key an, gibt Result an ClientSession zur�ck. M�gliche Error: KEY_NOT_FOUND
struct Result get(struct Command command);

//Gibt Key zur L�schung an Datenhaltung weiter, gibt Result an ClientSession zur�ck. M�gliche Error: KEY_NOT_FOUND
struct Result del(struct Command command);

//Beendet die Verbindung des ScoektClient
void quit(struct Command command);



#endif // APPLICATIONHEADER_H_INCLUDED
