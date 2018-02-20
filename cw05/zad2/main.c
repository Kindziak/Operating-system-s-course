#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <unistd.h>

int main(int argc, char ** argv) {
  pid_t masterPID, childPID;
  int status;

  if ((masterPID = fork()) == 0) {
    if (execl("./master","master","./data","600",NULL) == -1) {
      perror("Error");
      exit(1);
    }
  }
  for (int i = 0 ; i < 10 ; i++) {
    if ((childPID = fork()) == 0) {
      if(execl("./slave","slave","./data","1000000", "100", NULL) == -1) {
        perror("Error");
        exit(1);
      }
    }
  }
  waitpid(masterPID,&status,0);

  return 0;
}
