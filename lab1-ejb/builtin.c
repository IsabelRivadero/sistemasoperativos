#include "builtin.h"
#include "command.h"
#include "execute.h"
#include "tests/syscall_mock.h"

#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h> 
#include <stdio.h>
#include <sys/syscall.h>
#include "tests/syscall_mock.h"


bool exit_flag;

bool builtin_is_internal(scommand cmd){
    assert(cmd != NULL); 
    bool is_internal = false; 
    char* command_name = scommand_front(cmd);
    
    if(strcmp(command_name, "cd")  == 0){
        is_internal = true; 
    } 
    else if(strcmp(command_name, "exit") == 0){
        is_internal = true; 
    }
    return is_internal;
}

void builtin_exec(scommand cmd){
    assert(builtin_is_internal(cmd));
    char** command_array = NULL;
    command_array = scommand_copy_command(cmd);
    char* command_name = command_array[0];
    
    if(strcmp(command_name, "cd") == 0){
        if(chdir(command_array[1]) == -1){
            printf("crash: No such file or directory\n");
        }
    } else if(strcmp(command_name, "exit") == 0) {
        exit_flag = true;
    }
    command_array = scommand_command_array_destroy(command_array); 
}