#include "strextra.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

char * strmerge(char *s1, char *s2){
    assert(s1 != NULL &&  s2 != NULL);
    //char* str = malloc(strlen(s1)+strlen(s2)+1); 
    char* str=NULL;

    str=strcat(str, s1);
    str=strcat(str, s2);

    return str;
}