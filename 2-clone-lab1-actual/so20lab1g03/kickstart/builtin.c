#include "builtin.h"
#include "command.h"

#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool builtin_is_internal(scommand cmd){
    assert(cmd != NULL);

    return (strcmp(scommand_front(cmd), "cd")==0 || strcmp(scommand_front(cmd), "exit")==0);
}


void builtin_exec(scommand cmd){
    assert(builtin_is_internal(cmd));

    if(strcmp(scommand_front(cmd), "cd")==0){
        scommand_pop_front(cmd);
        char* command_path= scommand_front(cmd);
        chdir(command_path);
    }
    else if(strcmp(scommand_front(cmd), "exit")==0){
        exit(0);
    }
}
/*
builtin_exec ejecuta un comando interno, un comando interno no es un programa, son llamadas a sistema.
cd llama syscall , cd --> chdir(path), donde path seria el directorio especificado despues de cd
exit llama syscall exit , exit --> exit(0)
*/
