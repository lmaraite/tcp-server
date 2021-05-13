#ifndef APPLICATIONHEADER_H_INCLUDED
#define APPLICATIONHEADER_H_INCLUDED

//Entscheidet ob und welcher Befehl ausgeführt wird, gibt Result an ClientSession zurück. Mögliche Error: COMMAND_NOT_FOUND, MISSING_ARGUMENT, TO_MANY_ARGUMENTS
struct Result executeCommand(struct Command command);

//Gibt Key und Value an Datenhaltung zur speicherung weiter, gibt Result an ClientSession zurück.
struct Result Put(struct Command comm);

//Fragt den Value zum gegebenen Key an, gibt Result an ClientSession zurück. Mögliche Error: KEY_NOT_FOUND
struct Result get(struct Command command);

//Gibt Key zur Löschung an Datenhaltung weiter, gibt Result an ClientSession zurück. Mögliche Error: KEY_NOT_FOUND
struct Result del(struct Command command);

//Beendet die Verbindung des ScoektClient
void quit(struct Command command);



#endif // APPLICATIONHEADER_H_INCLUDED
