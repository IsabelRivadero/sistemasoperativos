#include "command.h"
#include "execute.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<sys/wait.h>

int main(int argc, char * argv[]){   
    pipeline apipe= pipeline_new();

    /*if (argc != 2) {   //////
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        exit(EXIT_FAILURE);
    }*/
    scommand comando= scommand_new();

    scommand_push_back(comando, "ls");
    //scommand_push_back(comando, "-l");
    
    //char * scommand_get_redir_in(comando); //get: muestre lo que tengo guardado
    //char * scommand_get_redir_out(comando);
    
    //scommand_set_redir_in(comando, " entrada"); //set: configura
    ///scommand_set_redir_out(comando, " salida");

    pipeline_push_back(apipe, comando);
///////////////////////////////////////////////////////////////////////////////////
   scommand comando2= scommand_new();

    scommand_push_back(comando2, "wc");
    scommand_push_back(comando2, "-l");
    

    pipeline_push_back(apipe, comando2);
    //////////////////////////////////////////////////////////////////////////
    scommand comando3= scommand_new();

    scommand_push_back(comando3, "grep");
    scommand_push_back(comando3, "1");
    

    pipeline_push_back(apipe, comando3);

    //////////////////////////////////////////////
    char * string= pipeline_to_string(apipe);
    printf("%s\n", string);

    execute_pipeline(apipe);
    
    return 0;
}

//para ejecutar con main mio2:
//gcc `pkg-config --cflags glib-2.0` -std=c99 -g -o test command.c builtin.c strextra.c execute.c main-mio2.c `pkg-config --libs glib-2.0`