#include "logger.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];

sem_t *mutex;
sem_t *empty;
sem_t *filled;

struct ThreadInfo {
  pthread_t thread_id;
  char thread_name[256];
};

void logger(const char *tag, const char *message, void *some_id, int dataType) {
  FILE *fptr;

  fptr = fopen("/Users/dharmikpatel/Documents/Coursework/OperationSystems/"
               "OS_project1/process.txt",
               "w");
  if (fptr == NULL) {
    perror("Error opening file");
  }
  time_t now;
  time(&now);
  switch (dataType) {
  case 1:
    fprintf(fptr, "%s [%s]: %s%s\n", ctime(&now), tag, message,
            ((char *)some_id));
    break;
  case 2:
    fprintf(fptr, "%s [%s]: %s%d\n", ctime(&now), tag, message,
            *((int *)some_id));
    break;
  default:
    fprintf(fptr, "%s [%s]: %s%d\n", ctime(&now), tag, message, 0);
  }

  fclose(fptr);
}

void initialize_sems() {
  /* sem_init is deprecated in OSX
  sem_init(&mutex, 0, 1);
  sem_init(&empty, 0, BUFFER_SIZE);
  sem_init(&filled, 0, 0);
  */
  mutex = sem_open("/mutex_sem", O_CREAT, 0, 1);
  empty = sem_open("/empty_sem", O_CREAT, 0, BUFFER_SIZE);
  filled = sem_open("/filled_sem", O_CREAT, 0, 0);
}

void thread_func(void *thread_name) {
  printf("Thread '%s' running\n", (char *)thread_name);
  logger("SUCCESS", "Thread is running: ", thread_name, 1);
}

void create_thread(const char *thread_name) {
  pid_t process_pid = getpid();
  pthread_t thread_id;

  // Create a new thread
  if (pthread_create(&thread_id, NULL, (void *(*)(void *))thread_func,
                     (void *)thread_name) == 0) {
    logger("SUCCESS", "Thread created successfully: ", &thread_name, 1);
    // You can add thread_id and thread_name to a data structure similar to your
    // Python code.
  } else {
    logger("ERROR", "Failed to create thread", 0, 2);
  }
}

void list_threads() {
  pid_t process_pid = getpid();
  printf("Process ID: %d\n", process_pid);

  struct ThreadInfo thread_data[100];

  for (int i = 0; i < 100; i++){
    if (thread_data[1].thread_id != 0) {
      printf("Thread ID: %lu, Name: %s\n", (unsigned long)thread_data[i].thread_id, thread_data[i].thread_name);
    }      
  }
}

int main() {
  create_thread("Thread1");
  create_thread("Thread2");

  initialize_sems();
  if (empty == SEM_FAILED) {
     perror("Failed to open semphore for empty");
     exit(-1);
  }
  
  list_threads();
  // Add more threads as needed
  // pthread_join() or other synchronization methods can be used to manage
  // thread execution
  return 0;
}
