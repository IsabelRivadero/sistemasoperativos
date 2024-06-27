#include "command.h"
#include "builtin.h"
#include "execute.h"



#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <glib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include "tests/syscall_mock.h"

static bool set_in_out(scommand command);
static void set_output_pipe(int* output_pipe, scommand command);
static void set_input_pipe(int* input_pipe, scommand command);
static void copy_pipe(int* copy_of_pipe, int* pipe);
static void launch_command(scommand command);
static void launch_external_command(scommand self);
static void wait_for_multiple_children(GArray* pid_array);
static void execute_multi_command_pipeline(pipeline apipe);

static void execute_single_command_pipeline(pipeline apipe);


static bool set_in_out(scommand command) {
    bool status  = true; 
    if(scommand_get_redir_in(command) != NULL) {
        int open_fd = open(scommand_get_redir_in(command), O_RDONLY, 0777); // El 0777 es solamente para que pase el test
        if(open_fd == -1) {
            char* in_file = strdup(scommand_get_redir_in(command));
            printf("crash: %s: No such file or directory\n", in_file);
            free(in_file);
            status = false;
        } else {
            close(0);
            dup(open_fd);
            close(open_fd);
        }
    }
    if(scommand_get_redir_out(command) != NULL) {
        int open_fd = open(scommand_get_redir_out(command), O_WRONLY | O_CREAT, 0777);
        close(1);
        dup(open_fd);
        close(open_fd);
    }
    return status;
}

static void set_input_pipe(int* input_pipe, scommand command) {
    if(scommand_get_redir_in(command) == NULL) {
        close(0);
        dup(input_pipe[0]);
    }
    close(input_pipe[0]);
    return;
}

static void set_output_pipe(int* output_pipe, scommand command) {
    if(scommand_get_redir_out(command) == NULL) {
        close(1);
        dup(output_pipe[1]);
    }
    close(output_pipe[0]);
    close(output_pipe[1]);
    return;
}

static void copy_pipe(int* copy_of_pipe, int* pipe) {
    copy_of_pipe[0] = pipe[0];
    copy_of_pipe[1] = pipe[1];
}

static void launch_command(scommand command) {
    if(builtin_is_internal(command)) {
       builtin_exec(command);
    } else {
        launch_external_command(command);
    }
}

static void launch_external_command(scommand self)
{
    char** command_array = scommand_copy_command(self);
    execvp(command_array[0], command_array);
    printf("crash: %s: command not found\n", command_array[0]);
    command_array = scommand_command_array_destroy(command_array);
}

static void wait_for_multiple_children(GArray* pid_array)
{
    while(pid_array->len > 0u) {
        pid_t pid_to_remove = wait(NULL);
        for(unsigned int index = 0u; index < pid_array->len; ++index) {
            if(pid_to_remove == g_array_index(pid_array, pid_t, index)) {
                g_array_remove_index_fast(pid_array, index);
            }
        }
    }
}

static void execute_multi_command_pipeline(pipeline apipe){
    GArray* pid_array = g_array_new(false, false, sizeof(pid_t));
    int output_pipe[2];
    pipe(output_pipe);
    pid_t pid = fork(); //Primer comando de la pipeline
    if(pid == 0) {
        scommand command = pipeline_front(apipe);
        set_in_out(command);
        set_output_pipe(output_pipe, command);
        launch_command(command);
    } else {
        g_array_append_val(pid_array, pid);
        close(output_pipe[1]);
        pipeline_pop_front(apipe);
        int input_pipe[2];
        while(pipeline_length(apipe) > 1u) { //Comandos entre el primer y ultimo comando
            copy_pipe(input_pipe, output_pipe);
            pipe(output_pipe);
            pid = fork(); 
            if(pid == 0) {  
                scommand command = pipeline_front(apipe);
                set_in_out(command);
                set_output_pipe(output_pipe, command);
                set_input_pipe(input_pipe, command);
                launch_command(command);
            }
            g_array_append_val(pid_array, pid);
            close(output_pipe[1]);
            close(input_pipe[0]);
            pipeline_pop_front(apipe);
        }
        copy_pipe(input_pipe, output_pipe);
        pid = fork(); // Ultimo comando
        if(pid == 0) {
            scommand command = pipeline_front(apipe);
            set_in_out(command);
            set_input_pipe(input_pipe, command);
            launch_command(command);
        } else {
            g_array_append_val(pid_array, pid);
            wait_for_multiple_children(pid_array);
            g_array_free(pid_array, true);
            pid_array = NULL;
            close(input_pipe[0]);
        }
    }
}

static void execute_single_command_pipeline(pipeline apipe){
    scommand command = pipeline_front(apipe);
    if(builtin_is_internal(command)) {
       builtin_exec(command);
    } else {
        pid_t pid = fork();
        if (pid == 0)
        {
            set_in_out(command);
            launch_external_command(command);
        }
        else if(pipeline_get_wait(apipe) && pid > 0)
        {
            wait(NULL);
        }
    }
}

void execute_pipeline(pipeline apipe){
    assert(apipe != NULL);
    if(pipeline_length(apipe) == 1)
        execute_single_command_pipeline(apipe);
    else if (pipeline_length(apipe) > 1)
        execute_multi_command_pipeline(apipe);
}