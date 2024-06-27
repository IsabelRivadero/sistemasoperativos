#include "command.h"
#include "strextra.h"
#include <assert.h>
//#include <glib-2.0/gmodule.h>
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


/* Un comando simple es una estructura que contará
con 3 campos, una lista con el comando y sus argumentos, y
dos campos de redirecciones uno para input y otro para output */

struct scommand_s {
    GList *list;
    char *redir_in;
    char *redir_out;
};

scommand scommand_new(void){
    scommand new = calloc(1, sizeof(struct scommand_s));
    new->list = NULL;
    new->redir_in = NULL;
    new->redir_out = NULL;
    return new;
}

scommand scommand_destroy(scommand self){
    assert(self!=NULL);
/* Comprobamos que self apunte hacia algo
Luego nos comprobamos uno a uno que no sean NULL
y se libera */
    if(self->list != NULL){
        g_list_free(self->list);
        self->list = NULL;
  }

    if(self->redir_in != NULL){
        free(self->redir_in);
        self->redir_in = NULL;
  }

    if(self->redir_out != NULL){
        free(self->redir_out);
        self->redir_out = NULL;
  }
    free(self);
    return NULL;
}

void scommand_push_back(scommand self, char * argument){
    assert(self!=NULL && argument!=NULL);
    self->list = g_list_append(self->list, argument);
}

/* Modificadores */

void scommand_pop_front(scommand self){
    assert(self!=NULL && !scommand_is_empty(self));
    self->list = g_list_remove(self->list, g_list_nth_data(self->list,0));
}

void scommand_set_redir_in(scommand self, char * filename){
    assert(self!=NULL);
    self->redir_in = filename;
}

void scommand_set_redir_out(scommand self, char * filename){
    assert(self!=NULL);
    self->redir_out = filename;
}

/* Proyectores */

bool scommand_is_empty(const scommand self){
    assert(self!=NULL);
    unsigned int length = 0u;
    if(self->list!=NULL)
        length = g_list_length(self->list);
    return length == 0u;
}

unsigned int scommand_length(const scommand self){
    assert(self!=NULL);
    return g_list_length(self->list);
}

char * scommand_front(const scommand self){
    assert(self!=NULL && !scommand_is_empty(self));
    char * front= g_list_nth_data(self->list,0);
    assert(self!=NULL);
    return front;
}

char * scommand_get_redir_in(const scommand self){
    assert(self!=NULL);
    return self->redir_in;
}

char * scommand_get_redir_out(const scommand self){
    assert(self!=NULL);
    return self->redir_out;
}

char * scommand_to_string(const scommand self){
    assert(self!=NULL);
    if(scommand_is_empty(self))
        return strmerge("","");

    char * result= "";
    for(unsigned int i=0; i<g_list_length(self->list); i++){
        result= strmerge(result, g_list_nth_data(self->list, i));
        if(i!=g_list_length(self->list)-1){
          result= strmerge(result, " ");
        }
    }
    //"0 1 2 .... N-3 < N-2 > N-1" la redireccion lo manda a un archivo
    if(self->redir_in!=NULL){
      result= strmerge(result, " < ");
      result= strmerge(result, self->redir_in);
    }
    if(self->redir_in!=NULL){
      result= strmerge(result, " > ");
      result= strmerge(result, self->redir_out);
    }

    return result;
}



/* Pipeline, es una secuencia de comandos, con lo que será
representado con una lista y a ello se le agrega otro campo
que cuenta con un booleano */
struct pipeline_s{
    GList * pipe_list;
    bool wait;
};

pipeline pipeline_new(){
    pipeline pipe = malloc(sizeof(pipeline));
    pipe->pipe_list = NULL;
    return pipe;
}

pipeline pipeline_destroy(pipeline self){
    assert(self!=NULL);
    scommand scmm;
    if(self->pipe_list != NULL){
        for(int i = pipeline_length(self)-1; i >= 0; --i){
            scmm = g_list_nth_data(self->pipe_list,i);
            scommand_destroy(scmm);
          }
        }
    free(self);
    return NULL;
}

void pipeline_push_back(pipeline self, scommand sc){
    assert(self!=NULL && sc!=NULL);
    self->pipe_list = g_list_append(self->pipe_list, sc);
}

void pipeline_pop_front(pipeline self){
    assert(self!=NULL && !pipeline_is_empty(self));
    self->pipe_list = g_list_delete_link (self->pipe_list, g_list_first (self->pipe_list));
}

void pipeline_set_wait(pipeline self, const bool w){
    assert(self!=NULL);
    self->wait = w;
}

bool pipeline_is_empty(const pipeline self){
    assert(self!=NULL);
    unsigned int length;
    length = pipeline_length(self);
    return (length == 0u);
}

unsigned int pipeline_length(const pipeline self){
    assert(self!=NULL);
    return g_list_length(self->pipe_list);
}


scommand pipeline_front(const pipeline self){
    assert(self!=NULL && !pipeline_is_empty(self));
    scommand result = g_list_nth_data(self->pipe_list,0);
    return result;
}

bool pipeline_get_wait(const pipeline self){
    assert(self!=NULL);
    return self->wait;
}

char * pipeline_to_string(const pipeline self){
    assert(self!=NULL);
    if(pipeline_is_empty(self)){
      return strmerge("", "");
    }

    char * result= "";
    unsigned int length= pipeline_length(self);

    for(unsigned int i=0; i<length; i++){
        result= strmerge(result, scommand_to_string(g_list_nth_data(self->pipe_list, i)));

        if(i!=length-1){
            result= strmerge(result, " | ");
        }
    }

//    char * b = pipeline_get_wait(self) ? "true" : "false";
//    result= strmerge(result, b);
      if(!pipeline_get_wait(self)){
        result= strmerge(result, " &");
      }

    return result;
}
