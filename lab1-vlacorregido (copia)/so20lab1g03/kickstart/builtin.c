#include "builtin.h"
#include "command.h"

#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool builtin_is_internal(scommand cmd){
    assert(cmd != NULL);
    //comprueba si cmd es un comando interno: 'cd' o 'exit'.

    return (strcmp(scommand_front(cmd), "cd")==0 || strcmp(scommand_front(cmd), "exit")==0) ;
}


void builtin_exec(scommand cmd){
    assert(builtin_is_internal(cmd)); 
    /*ejecuta un comando interno(que no es un programa), son llamadas a sistema.
    cd llama syscall , cd --> chdir(path), donde path seria el directorio especificado despues de cd
     exit llama syscall exit , exit --> exit(0)*/

    if(strcmp(scommand_front(cmd), "cd")==0){ //comparo dos cadenas de string 
        scommand_pop_front(cmd);              //elimino
        char* command_path= scommand_front(cmd); //me quedo con la direccion
        chdir(command_path);                     //y paso el path a la syscall chdir que cambia de directorio de trabajo (devuelve un entero)
    }
    else if(strcmp(scommand_front(cmd), "exit")==0){ //si es exit salgo
        exit(0);  //mejorar para hacerlo limpio??
    }
 
}
