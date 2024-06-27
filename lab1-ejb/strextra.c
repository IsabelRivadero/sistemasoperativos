#include <string.h>
#include <stdlib.h> 
#include "strextra.h"

char * strmerge(char *s1, char *s2)
{    
    size_t a = strlen(s1);
    size_t b = strlen(s2);
    size_t size_ab = a+b+1;
    s1 = realloc(s1, size_ab);
    s1 = strcat(s1,s2);
    return s1;
}