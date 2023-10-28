#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

// Structure to represent a process
struct Process {
    pid_t pid;
    char name[256];
};

// Array to store information about processes
struct Process processList[100];
int processCount = 0;

// Function to list running processes
void listProcesses() {
    printf("List of running processes:\n");
    for (int i = 0; i < processCount; i++) {
        printf("PID: %d, Name: %s\n", processList[i].pid, processList[i].name);
    }
}

// Function to create a new process
void createProcess(const char *command) {
    pid_t pid = fork();
    
    if (pid == 0) {
        // Child process
        execlp(command, command, NULL);
        exit(0); // Terminate the child process
    } else if (pid > 0) {
        // Parent process
        processList[processCount].pid = pid;
        snprintf(processList[processCount].name, sizeof(processList[processCount].name), "%s", command);
        processCount++;
    } else {
        // Fork failed
        perror("Fork failed");
    }
}

// Function to terminate a process by PID
void terminateProcess(pid_t pid) {
    if (kill(pid, SIGTERM) == 0) {
        printf("Process with PID %d terminated.\n", pid);
        // Remove the terminated process from the process list
        for (int i = 0; i < processCount; i++) {
            if (processList[i].pid == pid) {
                for (int j = i; j < processCount - 1; j++) {
                    processList[j] = processList[j + 1];
                }
                processCount--;
                break;
            }
        }
    } else {
        perror("Termination failed");
    }
}

int main() {
    while (1) {
        printf("Enter a command (create/list/terminate/exit): ");
        char command[256];
        scanf("%s", command);

        if (strcmp(command, "create") == 0) {
            printf("Enter the command to execute: ");
            char newCommand[256];
            scanf("%s", newCommand);
            createProcess(newCommand);
        } else if (strcmp(command, "list") == 0) {
            listProcesses();
        } else if (strcmp(command, "terminate") == 0) {
            printf("Enter the PID of the process to terminate: ");
            pid_t pid;
            scanf("%d", &pid);
            terminateProcess(pid);
        } else if (strcmp(command, "exit") == 0) {
            break;
        }
    }

    return 0;
}

