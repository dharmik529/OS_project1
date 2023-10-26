#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void create_process(){
  pid_t child_pid, wait_pid;
  int status;

  child_pid = fork(); //create new process 
                      
  if (child_pid < 0)
  {
    perror("Fork failed");
    exit(1);
  } else if (child_pid == 0) {
    //This is the child process
    printf("Child process (PID: %d) is running.\n", getpid());

    // Replace the child process with a different program using exec
    char *args[] = {"ls", "-l", NULL};
    execvp("ls", args);

    perror("Exec failed");
    exit(1);
  } else {
    // This is the parent process
    printf("Parent process (PID: %d) is waiting for the child process to complete.\n", getpid());

    wait_pid = wait(&status); // Wait for the child process to complete

    if(wait_pid == -1) {
      perror("Wait failed");
      exit(1);
    } else {
      if (WIFEXITED(status)) {
        printf("Child process (PID: %d) exited with status %d\n", wait_pid, WEXITSTATUS(status));
      } else if (WIFSIGNALED(status)) {
        printf("Child process (PID: %d) was terminated by signal: %d", wait_pid, WTERMSIG(status));
      }
    }
    printf("Parent process (PID: %d) is done.\n", getpid());
  }
}

int main() {
  create_process();

  return 0;
}
