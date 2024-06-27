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
#include "tests/syscall_mock.h" //falta corregir

void execute_pipeline(pipeline apipe){
    assert(apipe!=NULL);
    bool b= pipeline_get_wait(apipe); 
    
    unsigned int length_pipe=pipeline_length(apipe);
    if(length_pipe==0){ //si esta vacio no hago nada
        return;
    }

    if(!pipeline_is_empty(apipe)){       //si el pipeline no es vacio
        if(builtin_is_internal(pipeline_front(apipe))){ //si el comando es interno
            builtin_exec(pipeline_front(apipe));         //ejecuto y salgo, no sigo
            return;
        }
    }
    //inicializo redirecciones
    char * infile = NULL;   
    char * outfile = NULL;
    //dup duplica fd
    int tmpin= dup(0); //inicializo guardando fd por defecto
    int tmpout= dup(1);

    //set the initial input
    int fdin;
    //Obtiene los nombres de archivos a donde redirigir la entrada
    infile = scommand_get_redir_in(pipeline_front(apipe)); 
    if(infile != NULL)              //si hay algo, abro en modo lectura
      fdin = open(infile, O_RDONLY, 0); 
    else
    //sino asigno el fd estandar
      fdin = dup(STDIN_FILENO);

    int fdout;
    int pipefd[2]; //creo arreglo de pipes
    for(unsigned int i=0; i<length_pipe;i++){
        dup2(fdin, 0); //redirecciono a la entrada por defecto 
        close(fdin);
        pipe(pipefd);  //inicializo el arreglo de pipes

        pid_t pid; 
        pid= fork(); //creo un proceso hijo utilizando fork

        scommand first_scmd;
        first_scmd= pipeline_front(apipe); //el primer comand
        
        if(i == length_pipe-1){  //si es el ultimo comando
      	    outfile = scommand_get_redir_out(first_scmd);
            if(outfile != NULL)
                fdout = open(outfile, O_CREAT,0);
            else
         	    fdout = dup(tmpout);

        }else{ //si no soy el ultimo comando uso los pipes
            fdout= pipefd[1]; 
            fdin= pipefd[0];
        }
        
        dup2(fdout,1); //redirecciono a la salida standard
        close(fdout);
        
        if (pid < 0) { //error en el fork
            fprintf(stderr, "fork failed\n");
            exit(1);

        } else if (pid == 0) { 
         //quedo cerrar puntas de pipes que no necesito

            char **myargs= malloc((scommand_length(first_scmd) +1) *sizeof(char*));
            unsigned int length_command=scommand_length(first_scmd);
            //hago un arreglo de comandos
            for(unsigned int j=0; j<length_command ; j++){
                // guardo una copia concatenandola con vacio
                myargs[j]= strmerge(scommand_front(first_scmd), "");
                scommand_pop_front(first_scmd);  
            }
            myargs[length_command]= NULL;

            execvp(myargs[0], myargs); //si falla el hijo sigue hacer exit p ese caso

            // libero las memorias en un mal lugar, pero al hacer modificaciones no me funcionaba
            for(unsigned int  i=0; i<length_command; i++){ 
                free(myargs[i]);
            }

        } else { //proceso padre

            if(b)//consulto si el pipeline debe esperar
                wait(NULL);  //el padre espera a que algun hijo termine
        }                    //wget c2, no tiene que esperar que c1 termine y a medida que sigue empieza a leer- a fuera del for wait_pid: para que termine el ultimo hijo
    
    pipeline_pop_front(apipe);//elimino el primer comando simple  del pipe
    }
        dup2(tmpin, 0); //restauro fd por defecto
        dup2(tmpout,1);
        close(tmpin); //cierro pipes
        close(tmpout);
}

/*faltaria: -cerrar puntas intermedias
            -verificar redirecciones que no esten solo al ultimo o al principio
            -corregir lo de liberar memorias despues del execvp porque no se estan haciendo
            -corregir fallas que saltan en los mock_test
            */ 