#include "execute.h"
#include "command.h"
#include "strextra.h"
#include "builtin.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "tests/syscall_mock.h" //lo agrego?


void execute_scommand(scommand cmd) {
    // Redireccionar la entrada y la salida correspondiente
    // contruyo la variable myargs con el for(unsigned int j=0; j<length_command ; j++)
    // execvp(myargs[0], myargs);  Después de este punto, la ejecución no vuelve nunca 
    //al hijo a menos que falle execvp
    
    int fdin, fdout;
    char * infile = NULL;
    char * outfile = NULL;

    infile = scommand_get_redir_in(cmd);
    if(infile != NULL){
        fdin = open(infile, O_RDONLY, 0); 
        dup2(fdin, 0);
        close(fdin);
    }

    outfile = scommand_get_redir_out(cmd);
    if(outfile != NULL){
        fdout = open(outfile, O_CREAT,0);
        dup2(fdout, 1);
        close(fdout);
    }
    
    char **myargs= malloc((scommand_length(cmd) +1) *sizeof(char*));
    unsigned int length_command=scommand_length(cmd);
            
    for(unsigned int j=0; j<length_command ; j++){
        strcpy(myargs[j], scommand_front(cmd));/////??? strmerge esta bien implementado?
        scommand_pop_front(cmd);  
    }
    myargs[length_command]= NULL;

    execvp(myargs[0], myargs); 
    perror( "Command failed\n");
    exit(0);

}

/*void execute_pipeline(pipeline apipe) {
    for(unsigned int i=0; i<length_pipe;i++) {
        fork();
        if (estoy en el hijo) {
              // duplico la entrada y salida al pipe correspondiente 
              // cierrro los extremos de todos los pipes que estén abiertos
              //que no voy a utilizar en este proceso hijo
              // llamo a execute_scommand
        } else if (estoy en el padre) {
              pipeline_pop_front(apipe);  // esto también estaba bien
        }
    }
    // Después de haber creado  todos los hijos, me fijo si es necesario esperar con waitpid
}*/

// duplico la entrada y salida al pipe correspondiente
// cierrro los extremos de todos los pipes que estén abiertos que no voy a utilizar en este proceso hijo
void execute_pipeline(pipeline apipe){
    assert(apipe!=NULL);
    bool b= pipeline_get_wait(apipe);
    
    unsigned int length_pipe=pipeline_length(apipe);
    if(length_pipe==0){
        return;
    }

    if(builtin_is_internal(pipeline_front(apipe))){
        builtin_exec(pipeline_front(apipe));
    }

    int pipefd1[2], pipefd2[2];
    if(length_pipe>1){
        pipe(pipefd1);
        pipe(pipefd2);
    }
    
    int fdin, fdout;
    pid_t pid;
    for(unsigned int i=0; i<length_pipe;i++){
        scommand first_scmd= pipeline_front(apipe);
        pid= fork();

        if (pid < 0) { 
            perror("fork failed\n");
            exit(1);

        } else if (pid == 0) { 
            if(length_pipe>1){
                if(i==length_pipe- 1){ //ultimo de pipeline de mas de un comando
                fdin= i%2 == 0 ? pipefd2[0] : pipefd1[0];
            
                close(i%2 == 0 ? pipefd1[0] : pipefd2[0]);
                close(pipefd1[1]);
                close(pipefd2[1]);

            }else if(i==0 && (length_pipe > 1)){
                fdout=pipefd1[1]; //soy el primer comando y hay otro comando
                close(pipefd1[0]);
                close(pipefd2[1]);
                close(pipefd2[0]);

            }else{ //si no soy el ultimo comando uso los pipes
                if(i%2==0){
                    fdout=pipefd1[1];
                    fdin= pipefd2[0];
                    close(pipefd1[0]);
                    close(pipefd2[1]);
                }else {
                    fdout=pipefd2[1];
                    fdin= pipefd1[0];
                    close(pipefd1[1]);
                    close(pipefd2[0]);
                }
            }
            dup2(fdin, STDIN_FILENO); //redirecciono a la entrada por defecto
            close(fdin);
        
            dup2(fdout,STDOUT_FILENO); //redirecciono la salida estandard a un archivo o lo que este este en fdout
            close(fdout);
        }
        execute_scommand(first_scmd);

        } else { //padre
            if(length_pipe > 1){
                close(pipefd1[1]);
                close(pipefd1[0]);
                close(pipefd2[1]);
                close(pipefd2[0]);
            }
        }

    pipeline_pop_front(apipe);//elimino el primer comando simple  del pipe

    /*for(unsigned int  i=0; i<length_command; i++){ //
        free(myargs[i]);
    }*/
    }
    if(b)
        waitpid(pid, NULL, 0); //el ultimo valor del pid es el del ultimo hijo que queremos esperar
    
}
//wget c2, no tiene que esperar que c1 termine y a medida que sigue empieza a leer- a fuera del for wait_pid: para que termine el ultimo hijo