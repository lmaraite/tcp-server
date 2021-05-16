#ifndef APPLICATIONHEADER_H_INCLUDED
#define APPLICATIONHEADER_H_INCLUDED

//public
/*
       Wenn Result.error_code = SUCCESS steht im Result.value der Wert den der
       Client auf der Konsole sehen soll.
       Ansonsten wird nur der error_code im Result �bermittelt und das value
       bleibt LEER.
*/
//Entscheidet ob und welcher Befehl ausgef�hrt wird, gibt Result an ClientSession zur�ck. M�gliche Error: COMMAND_NOT_FOUND
struct Result executeCommand(struct Command command);
//private

//Gibt Key und Value an Datenhaltung �ber "save()" weiter und gibt result an "executeCommand()" zur�ck.
struct Result put(char key, char value);

//Fragt den Value zum gegebenen key per "get_by_key()" an und gibt result an "executeCommand()" zur�ck. M�gliche Error: KEY_NOT_FOUND
struct Result get(char key);

//Gibt Key zur L�schung an Datenhaltung per "delete()" weiter, gibt Result an "executeCommand()" zur�ck. M�gliche Error: KEY_NOT_FOUND
struct Result del(char key);

//Beendet die Verbindung des ScoektClient.
void quit();



#endif // APPLICATIONHEADER_H_INCLUDED
