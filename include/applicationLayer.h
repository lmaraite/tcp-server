#ifndef APPLICATIONHEADER_H_INCLUDED
#define APPLICATIONHEADER_H_INCLUDED

//public
/*
       Wenn Result.error_code = SUCCESS steht im Result.value der Wert den der
       Client auf der Konsole sehen soll.
       Ansonsten wird nur der error_code im Result übermittelt und das value
       bleibt LEER.
*/
//Entscheidet ob und welcher Befehl ausgeführt wird, gibt Result an ClientSession zurück. Mögliche Error: COMMAND_NOT_FOUND
struct Result executeCommand(struct Command command);
//private

//Gibt Key und Value an Datenhaltung über "save()" weiter und gibt result an "executeCommand()" zurück.
struct Result put(char key, char value);

//Fragt den Value zum gegebenen key per "get_by_key()" an und gibt result an "executeCommand()" zurück. Mögliche Error: KEY_NOT_FOUND
struct Result get(char key);

//Gibt Key zur Löschung an Datenhaltung per "delete()" weiter, gibt Result an "executeCommand()" zurück. Mögliche Error: KEY_NOT_FOUND
struct Result del(char key);

//Beendet die Verbindung des ScoektClient.
void quit();



#endif // APPLICATIONHEADER_H_INCLUDED
