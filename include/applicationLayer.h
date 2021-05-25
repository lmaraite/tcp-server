#ifndef APPLICATIONHEADER_H_INCLUDED
#define APPLICATIONHEADER_H_INCLUDED

/*
       Wenn Result.error_code = SUCCESS steht im Result.value der Wert den der
       Client auf der Konsole sehen soll.
       Ansonsten wird nur der error_code im Result �bermittelt und das value
       bleibt LEER.
*/
//Entscheidet ob und welcher Befehl ausgef�hrt wird, gibt Result an ClientSession zur�ck. M�gliche Error: COMMAND_NOT_FOUND
struct Result executeCommand(struct Command command);

#endif // APPLICATIONHEADER_H_INCLUDED
