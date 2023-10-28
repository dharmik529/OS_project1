#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

void create_process(char *args[]){
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
    // char *args[] = {"ls", "-l", NULL};
    char *cmd = args[0];
    execvp(cmd, args);

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

void listProcess() {
  printf("List of running processes:\n");
  system("ps -e");
}

void terminateProcess(int pid) {
  int result = kill(pid, SIGTERM);
  if (result == 0) {
    printf("Process with PID %d terminted succesfully.\n", pid);
  } else {
    perror("Failed to terminate the process");
  }
}

void monitorProcess(int pid) {
  int status;
  pid_t wait_pid;

  wait_pid = waitpid(pid, &status, 0);

  if (wait_pid == -1) {
    perror("Wait for process failed");
  } else {
    if (WIFEXITED(status)) {
      printf("Process with PID %d exited with status: %D\n", wait_pid, WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
      printf("Process with PID %d was terminated by signal: %d\n", wait_pid, WTERMSIG(status));
    }
  }
}

int main(int argc, char *argv[]) {

  char *args[argc];
  /*
  printf("Arg Count: %d\n", argc);
  printf("Args: ");
  for (int i = 0; i < argc; ++i){
    printf("%s ", argv[i]);
    args[i] = argv[i];
  }

  for (int i = 1; i < argc; ++i){
    args[i-1] = argv[i];
  }
  args[argc-1] = NULL;

  create_process(args);
*/

  int choice, pid;

  while(1) {
    printf("\nSelect an option:\n");
    printf("1. List processes\n");
    printf("2. Terminate a process\n");
    printf("3. Monitor a process\n");
    printf("4. Create a process\n");
    printf("4. Exit\n");

    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
      case 1:
        listProcess();
        break;
      case 2:
        printf("Enter the PID of the process to terminate: ");
        scanf("%d", &pid);
        terminateProcess(pid);
        break;
      case 3:
        printf("Enter the PID of the process to monitor: ");
        scanf("%d", &pid);
        monitorProcess(pid);
        break;
      case 4:
        printf("Arg Count: %d\n", argc);
        printf("Args: ");
        for (int i = 0; i < argc; ++i){
          printf("%s ", argv[i]);
          args[i] = argv[i];
        }

        for (int i = 1; i < argc; ++i){
          args[i-1] = argv[i];
        }
        args[argc-1] = NULL;

        create_process(args);
      case 5:
        exit(0);
      default:
        printf("Invalid choice. Please try again.\n");
    }
  }
  return 0;
}
