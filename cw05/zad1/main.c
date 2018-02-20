#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void exitOrError(int i) {
  switch (i) {
    case 1: printf("ERROR! Niepoprawne wykonanie polecenia getline!\n"); break;
    case 2: printf("Dziekujemy za skorzystanie z naszego programu!\n"); exit(0); break;
    case 3: printf("ERROR ! Niepoprawne wykonanie polecenia pipe!\n"); break;
    case 4: printf("ERROR ! Niepoprawne wykonanie polecenia fork!\n"); break;
    case 5: printf("ERROR ! Niepoprawne wykonanie polecenia execvp!\n"); break;
    case 6: printf("ERROR ! Nie ma czego wykonywac!\n"); break;
    default: printf("Brak takiego bledu!\n"); break;
  }
  exit(1);
}

void inParent(char ** commandArguments, int * fd, int input, int output) {
  if (input != -1) dup2(input, 0);
  if (output != -1) dup2(fd[1], 1);
  if (execvp(commandArguments[0], commandArguments) == -1) exitOrError(5);
}

void inChild(int * fd, int input) {
  close(fd[1]);
  dup2(fd[0],0);
  wait(NULL);
  if (input != -1) close(input);
}

void doCommandInProcess(char ** commandArguments, int * fd, int input, int output) {
  pid_t child;

  if ((child = fork()) < 0) exitOrError(4);
  else if (child == 0) inParent(commandArguments, fd, input, output);
  else inChild(fd, input);

  free(commandArguments);
}

char ** splitCommandArguments(char * command, char ** commandArguments) {
  commandArguments[0] = strtok(command," \n\0");
  for (int i = 1 ; i < 20 && commandArguments[i-1] != NULL ; i++) {
    commandArguments[i] = strtok(NULL," \n\0");
  }
  return commandArguments;
}

void doCommand(char * command, int * fd, int input, int output) {
  char ** commandArguments = calloc(3,sizeof(char *));

  commandArguments = splitCommandArguments(command, commandArguments);
  doCommandInProcess(commandArguments,fd,input,output);
}

void prepareCommand(char ** lineCommands) {
  int fd[2];
  int prevOutput = STDIN_FILENO;

  if (pipe(fd) < 0) exitOrError(3);
  for (int i = 0 ; i < 20 && lineCommands[i] != NULL ; i++) {
    if (pipe(fd) < 0) exitOrError(3);
    if (i+1 == 20 || lineCommands[i+1] == NULL) doCommand(lineCommands[i],fd,prevOutput,-1);
    else doCommand(lineCommands[i],fd,prevOutput,0);
    prevOutput = fd[0];
    close(fd[1]);
  }
  close(fd[0]);
  free(lineCommands);
}

char ** splitLineIntoCommands(char * line, char ** lineCommands) {
  lineCommands[0] = strtok(line,"|");
  for (int i = 1 ; i < 20 && lineCommands[i-1] != NULL ; i++) {
    lineCommands[i] = strtok(NULL,"|");
  }
  return lineCommands;
}

void doLineCommands(char * line) {
  char ** lineCommands = calloc(20,sizeof(char *));
  lineCommands = splitLineIntoCommands(line, lineCommands);
  prepareCommand(lineCommands);
}

char * loadLine(char * line) {
  size_t lineSize = 0;

  printf("Wpisz nastepna linie polecen do wykonania lub 1 aby wyjsc.\n");
  if (getline(&line,&lineSize,stdin) < 0) exitOrError(1);
  if (strcmp(line,"1\n") == 0) exitOrError(2);
  if (strcmp(line,"\n") == 0) exitOrError(6);

  return line;
}

int main() {
  char * line;

  for (;;) {
    line = loadLine(line);
    doLineCommands(line);
    line = NULL;
    break;
  }

  return 0;
}
