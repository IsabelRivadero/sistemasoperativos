#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "builtin.h"
#include "execute.h"
#include "command.h"

void execute_pipeline(pipeline apipe){
   assert(apipe!=NULL);

    //comando y pipeline temporales para modificar.
   scommand current_command;
   pipeline tmp_pipe = apipe;

    //Input y Output del comando
    char * infile = NULL;
    char * outfile = NULL;

    //se inicializa la entrada comprobando si hay o no un redirección de entrada
    //en caso de que la haya abrirá el archivo y lo guardará en fdin.
    int fdin;
    infile = scommand_get_redir_in(pipeline_front(apipe));
    if(infile != NULL){
      fdin = open(infile, O_RDONLY);
    }
    else{
    //en caso de que no haya redireccion, le asigna la estandar
      fdin = dup(0);
    }

    int ret, fdout;

    //este ciclo for creará un proceso para cada comando simple
    for(unsigned int i=0u; i<pipeline_length(apipe); i++){

      //redirige la entrada estándar para que provenga de fdin
      //y luego cierra el file descriptor
      dup2(fdin, 0);
      close(fdin);
      //se crea un comando temporal para ir avanzando en el pipeline,
      //ejecutarlos e ir liberandolos
      current_command = pipeline_front(tmp_pipe);
      pipeline_pop_front(tmp_pipe);
      //commprueba si el ultimo comando contiene redirección de salida
      //si es el caso se lo asigna a outfile de lo contrario
      //se le asigna la salida estandar
      if(i == pipeline_length(apipe)-1){
         outfile = scommand_get_redir_out(current_command);

         if(outfile != NULL){
           fdout=open(outfile, O_CREAT);
         }

         else{
            fdout=dup(1);
         }
      }

      else{
        //para el resto de comandos la salida y entrada va a ser pasada
        //a traves de un pipe donde se comunican entre el comando analizado
        //y el comando que sigue (lo que se escriba en uno se lee en el otro)
        int fdpipe[2];
        pipe(fdpipe);
        fdout=fdpipe[1];
        fdin=fdpipe[0];
      }
      //se redireciona y cierra la salida
      dup2(fdout,1);
      close(fdout);
      //se crea un proceso hijo con fork
      ret=fork();
      //si da un numero negativo hubo un error al crear el fork
      if(ret<0){
        fprintf(stderr, "Execute error\n");
        exit(1);
      }

      //si el fork da 0, ese proceso es el hijo
      else if(ret==0){
      //comprueba si es un comando interno y lo ejecuta
      if(builtin_is_internal(current_command)){
        builtin_exec(current_command);
      }
      //se crean e inicializan variables y se les asigna memoria
      //correspondiente para analizar y ejecutar comandos
      char * first = scommand_front(current_command);

      char ** command = malloc(sizeof(current_command)+1);
      char * parsed;
      unsigned int i=0u;
      scommand scmd_tmp = current_command;

      parsed=scommand_front(current_command);
      //se almacenan los comandos en una lista del
      //formato que necesita execvp para funcionar
        while(parsed != NULL){
          command[i]=parsed;
          i++;
          scommand_pop_front(scmd_tmp);
          parsed = scommand_front(scmd_tmp);
        }
        //se ejecuta el comando y al finalizar se termina y libera memoria
        execvp(first,command);
        perror("execvp");
        _exit(1);
        free(first);
        free(command);
        free(parsed);

      }
   }
}
