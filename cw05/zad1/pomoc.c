#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_CMD_NO 20
#define MAX_ARGS_NO 5

const char* SEPARATOR = "|";

void execute_line(char* line);
void execute_cmd(char* cmd,int* fd,int input, int output);

int main(int argc, char *argv[]){

    char *line = NULL;
    size_t size = 0;

    getline(&line, &size, stdin);
    execute_line(line);

    return 0;
}


void execute_line(char* line){
    char** cmd_array = calloc(MAX_CMD_NO,sizeof(char*));

    cmd_array[0] = strtok(line,SEPARATOR);
    for (int i = 1; i < MAX_CMD_NO && cmd_array[i-1] != NULL ; ++i){
        cmd_array[i] = strtok(NULL,SEPARATOR);
    }

    int fd[2];
    pipe(fd);
    int last_output = STDIN_FILENO;
    for (int i = 0; i < MAX_CMD_NO && cmd_array[i] != NULL ; ++i){
        pipe(fd);
        i + 1 == MAX_CMD_NO || cmd_array[i+1] == NULL ? execute_cmd(cmd_array[i],fd,last_output,-1) : execute_cmd(cmd_array[i],fd,last_output,1);
        last_output = fd[0];
        close(fd[1]);
    }
    close(fd[0]);

    free(cmd_array);


}

void execute_cmd(char* cmd,int* fd,int input, int output){
    char** cmd_args = calloc(MAX_ARGS_NO,sizeof(char*));

    cmd_args[0] = strtok(cmd," \n\t\r");
    for (int i = 1; i < MAX_ARGS_NO && cmd_args[i-1] != NULL ; ++i){
        cmd_args[i] = strtok(NULL," \n\t\r");
    }

    int x = fork();
    if (x == 0){
         if(input != -1) dup2(input,0);
         if(output != -1) dup2(fd[1],1);
         if(execvp(cmd_args[0], cmd_args) == -1){
            fprintf(stderr, "Could not execute command %s\n", cmd_args[0]);
            exit(1);
        }
    }
    else if(x > 0){
        close(fd[1]);
        dup2(fd[0],0);
        wait(NULL);
        if(input != -1) close(input);
    }
    else {
        fprintf(stderr, "Could not fork \n");
        exit(1);
    }


    free(cmd_args);


}
