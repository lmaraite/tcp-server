#ifndef DATEN_HALTUNG_HEADER
#define DATEN_HALTUNG_HEADER

// public
/*
    In Result.value steht das value des keys, wenn er gefunden wurde
    ansonsten bleibt value leer und in Result.error_code steht KEY_NOT_FOUND
*/
struct Result find_by_key(char* key);

/*
    Gibt SUCCESS oder eine Fehlermeldung über 1000 zurück
*/
int save(char* key, char* value);

/*
    Gibt SUCCESS oder eine Fehlermeldung über 1000 zurück
*/

int delete_by_key(char* key);

#endif
