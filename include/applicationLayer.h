#ifndef APPLICATIONHEADER_H_INCLUDED
#define APPLICATIONHEADER_H_INCLUDED

/*
       Wenn Result.error_code = SUCCESS steht im Result.value der Wert den der
       Client auf der Konsole sehen soll.
       Ansonsten wird nur der error_code im Result übermittelt und das value
       bleibt LEER.
*/
//Entscheidet ob und welcher Befehl ausgeführt wird, gibt Result an ClientSession zurück. Mögliche Error: COMMAND_NOT_FOUND
struct Result executeCommand(struct Command command);

#endif // APPLICATIONHEADER_H_INCLUDED
