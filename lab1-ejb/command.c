#include <assert.h>
#include <glib.h>
#include <gmodule.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "command.h"
#include "strextra.h"

// Scommand
struct scommand_s
{
    GQueue * command;
    char* in;
    char* out;
};

scommand scommand_new(void) {
    scommand s = calloc(1,sizeof(struct scommand_s));
    s->command = g_queue_new();
    s->in = NULL;
    s->out = NULL;
    return s;
}

scommand scommand_destroy(scommand self){
    assert(self!= NULL);
    g_queue_free_full(self->command,free);
    self->command = NULL;
    if(self->in != NULL) {
        free(self->in);
        self->in = NULL;
    }
    if (self->out != NULL) {
        free(self->out);
        self->out = NULL;
    }
    free(self);
    self = NULL;
    return self;
}

/* Modificadores */

void scommand_push_back(scommand self, char* argument){
    assert((self != NULL) && (argument != NULL));
    g_queue_push_tail(self->command, argument);
}

void scommand_pop_front(scommand self){
    assert((self != NULL) && (!scommand_is_empty(self)));
    gpointer trash = g_queue_pop_head(self->command);
    free(trash);
}

void scommand_set_redir_in(scommand self, char * filename){
    assert(self!=NULL);
    free(self->in);
    self->in = filename;
}

void scommand_set_redir_out(scommand self, char * filename){
    assert(self!=NULL);
    free(self->out);
    self->out = filename; 
}

bool scommand_is_empty(const scommand self){
    assert(self!=NULL);
    return g_queue_is_empty(self->command);
}

unsigned int scommand_length(const scommand self){
    assert(self != NULL);
    return g_queue_get_length(self->command);
}

char * scommand_front(const scommand self){
    assert(self!=NULL && !scommand_is_empty(self));
    return (char*) g_queue_peek_head(self->command);
}

char * scommand_get_redir_in(const scommand self){
    assert(self != NULL);
    return self->in;
}

char * scommand_get_redir_out(const scommand self){
    assert(self!= NULL);
    return self->out;
}

char** scommand_command_array_destroy(char** command_array){
    unsigned int i = 0u;
    while(command_array[i] != NULL)
    {
        free(command_array[i]);
        command_array[i] = NULL;
        i++;
    }
    free(command_array[i]);
    free(command_array);
    command_array = NULL;
    return command_array;
}

char** scommand_copy_command(scommand self){
    assert(!scommand_is_empty(self));
    GQueue* aux = g_queue_copy(self->command);
    char** result = calloc(g_queue_get_length(aux)+1, sizeof(char*));
    unsigned int i = 0u;
    char* arg = NULL;
    while(!g_queue_is_empty(aux))
    {
        arg = (char*)g_queue_pop_head(aux);
        result[i] = calloc(strlen(arg)+1, sizeof(char));
        char* auxarg = strdup(arg);
        result[i] = auxarg;
        ++i;
    }
    g_queue_free_full(aux, free);
    aux = NULL;
    result[i] = NULL;
    return result;
}

char * scommand_to_string(const scommand self){
    assert(self != NULL);
    char* result = malloc(1);
    result = strcat(result, "");
    char* aux = NULL;
    if(scommand_length(self) > 0u){
        GQueue* q = g_queue_copy(self->command);
        while (!g_queue_is_empty(q)){
            aux = g_queue_pop_head(q);
            if (g_queue_get_length(q) == 0u){ //caso en el que es el ultimo elemento
                result = strmerge(result, aux);
            } else{
                result = strmerge(result, aux);
                char* space = strdup(" ");
                result = strmerge(result, space);
                free(space);
            }
        }
        g_queue_free_full(q,free);
        if(self->in != NULL){
            char* redir_in = strdup(" < "); 
            result = strmerge(result, redir_in);
            result = strmerge(result, self->in);
            free(redir_in);
        }

        if(self->out != NULL){
            char* redir_out = strdup(" > ");
            result = strmerge(result, redir_out);
            result = strmerge(result, self->out);
            free(redir_out);
        }
    }
    return result;
}

// Pipeline
struct pipeline_s{
    GQueue* commands;
    bool wait;
};

pipeline pipeline_new(void){
    pipeline new = calloc(1,sizeof(struct pipeline_s));
    new->commands = g_queue_new();
    new->wait = true;
    return new;
}

pipeline pipeline_destroy(pipeline self){
    assert(self != NULL);
    scommand aux = NULL;
    if (!pipeline_is_empty(self)){
        while(!g_queue_is_empty(self->commands)) {
            aux = g_queue_pop_head(self->commands);
            aux = scommand_destroy(aux);
        }
    }
    g_queue_free_full(self->commands, free); 
    free(self);
    self = NULL;
    return self;
}

void pipeline_push_back(pipeline self, scommand sc){
    assert(self!=NULL && sc!=NULL);
    g_queue_push_tail(self->commands, sc);
}

void pipeline_pop_front(pipeline self){
    assert(self!=NULL && !pipeline_is_empty(self));
    scommand trash = g_queue_pop_head(self->commands);
    trash = scommand_destroy(trash);
}

void pipeline_set_wait(pipeline self, const bool w){
    assert(self != NULL);
    self->wait = w;
}

bool pipeline_is_empty(const pipeline self){
    assert(self!=NULL);
    return g_queue_is_empty(self->commands);
}

unsigned int pipeline_length(const pipeline self){
    assert(self!=NULL);
    return g_queue_get_length(self->commands);
}

scommand pipeline_front(const pipeline self){
    assert(self!=NULL && !pipeline_is_empty(self));
    return (scommand) g_queue_peek_head(self->commands);
}

bool pipeline_get_wait(const pipeline self){
    assert(self != NULL);
    return self->wait;
}

char * pipeline_to_string(const pipeline self){
    assert(self != NULL);
    char* result = strdup("\0");
    char* aux = NULL;
    if(!pipeline_is_empty(self))
    {
        GQueue* q = g_queue_copy(self->commands);
        while (!g_queue_is_empty(q)){
            aux = scommand_to_string(g_queue_pop_head(q));
            if (g_queue_get_length(q) == 0u) {
                result = strmerge(result, aux);
            } else {
                result = strmerge(result, aux);
                char* pipe = strdup(" | ");
                result = strmerge(result, pipe);
                free(pipe);
            }
            free(aux);
        }
        g_queue_free_full(q,free);
        if (!self->wait){
            char* ampersand = strdup(" & ");
            result = strmerge(result, ampersand);
            free(ampersand);
        }
    }
    return result;
}
