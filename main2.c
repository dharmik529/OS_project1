#include "logger.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int in = 0, out = 0;

int pipe_fd[2];

int shared_queue[100];

int read_pipe, write_pipe;

pthread_mutex_t mutex;

sem_t mutex_sem;
sem_t empty_sem;
sem_t filled_sem;

struct ThreadInfo {
  pthread_t thread_id;
  char thread_name[256];
};

struct ProcessInfo {
  int pid;
  char process_name[256];
};

struct ThreadInfo thread_data[100];

struct ProcessInfo process_data[100];

void logger(const char *tag, const char *message, void *some_id, int dataType) {
  FILE *fptr;

  fptr = fopen("./process.txt", "a");
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

void empty_log_file() {
  FILE *fptr;

  fptr = fopen("./process.txt", "w");
  if (fptr == NULL) {
    perror("Error opening file");
  }
  time_t now;
  time(&now);

  fprintf(fptr, "");

  fclose(fptr);
}

void ipc_send_message(const char *message) {
  write(write_pipe, message, strlen(message));
  logger("INFO", "Message Sent", 0, 2);
}

char *ipc_recieve_message() {
  char buffs[1024];
  int flags = fcntl(read_pipe, F_GETFL);
  fcntl(read_pipe, F_SETFL, flags | O_NONBLOCK);

  ssize_t bytes_read = read(read_pipe, buffer, sizeof(buffs));

  if (bytes_read == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      // Handle non-blocking read by returning NULL if no message is available
      return NULL;
    } else {
      perror("Error reading from the pipe");
      return NULL;
    }
  } else if (bytes_read == 0) {
    // Pipe has been closed
    return NULL;
  } else {
    buffer[bytes_read] = '\0'; // Null-terminate the received message
    return strdup(buffs);     // Return a dynamically allocated string
  }
}

void initialize_sems() {
  // sem_init is deprecated in OSX
  sem_init(&mutex_sem, 0, 1);
  sem_init(&empty_sem, 0, BUFFER_SIZE);
  sem_init(&filled_sem, 0, 0);
  
  /*
  mutex_sem = sem_open("/mutex_sem", O_CREAT, 0, 1);
  empty_sem = sem_open("/empty_sem", O_CREAT, 0, BUFFER_SIZE);
  filled_sem = sem_open("/filled_sem", O_CREAT, 0, 0);
  */
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
    for (int i = 0; i < 100; i++) {
      if (thread_data[i].thread_id == 0) {
        thread_data[i].thread_id = thread_id;
        strncpy(thread_data[i].thread_name, thread_name,
                sizeof(thread_data[i].thread_name));
        break;
      }
    }
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

  for (int i = 0; i < 100; i++) {
    if (thread_data[i].thread_id != 0) {
      printf("Thread ID: %lu, Name: %s\n",
             (unsigned long)thread_data[i].thread_id,
             thread_data[i].thread_name);
    }
  }
}

void process_menu(char *process_name) {
  pid_t pid = getpid();
  logger("INFO", "Child process is running: ", &pid, 2);

  while (1) {
    printf("Options within the process:\n");
    printf("1. Create a thread\n");
    printf("2. List threads\n");
    printf("3. Exit process\n");
    printf("Select an option: ");

    char choice[256];
    if (fgets(choice, sizeof(choice), stdin)) {
      choice[strcspn(choice, "\n")] = '\0'; // Remove the newline character

      if (strcmp(choice, "1") == 0) {
        char thread_name[256];
        printf("Enter a name for the thread: ");
        if (fgets(thread_name, sizeof(thread_name), stdin)) {
          thread_name[strcspn(thread_name, "\n")] =
              '\0'; // Remove the newline character
          create_thread(
              thread_name); // Call create_thread to create a new thread
          printf("\n");
        }
      } else if (strcmp(choice, "2") == 0) {
        list_threads(); // Call list_threads to display the threads in this
                        // process
        printf("\n");
      } else if (strcmp(choice, "3") == 0) {
        printf("Exited process.\n");
        break;
      } else {
        printf("Invalid option. Try again.\n");
      }
    }
  }
}

void create_process(char *process_name) {
  pid_t pid = fork();

  if (pid == 0) {
    pthread_mutex_lock(&mutex);

    char *argv[] = {process_name, NULL};
    char *const envp[] = {NULL};

    execvp(process_name, argv);
    for (int i = 0; i < 100; i++) {
      if (process_data[i].pid == 0) {
        process_data[i].pid = pid;
        strncpy(process_data[i].process_name, process_name,
                sizeof(process_data[i].process_name));
        break;
      }
    }

    perror("Error");
    pthread_mutex_unlock(&mutex);
    exit(1);
  } else if (pid < 0) {
    logger("ERROR", "Forked Failed", 0, 2);
  } else {
    logger("SUCCESS", "Child process created PID: ", &pid, 2);
    process_menu(process_name);
  }
}

void terminate_thread(const char *thread_name) {
  pthread_t self_thread_id = pthread_self();
  pid_t process_pid = getpid();

  pthread_t threads_to_remove[100];
  int num_threads_to_remove = 0;

  for (int i = 0; i < 100; i++) {
    if (thread_data[i].thread_id != 0 && process_pid == getpid()) {
      if (strcmp(thread_data[i].thread_name, thread_name) == 0) {
        if (pthread_cancel(thread_data[i].thread_id) == 0) {
          logger("INFO", "Terminatio requested for thread: ", &thread_name, 1);
          threads_to_remove[num_threads_to_remove++] = thread_data[i].thread_id;
        } else {
          logger("ERROR", "Failed termination for thread: ", &thread_name, 1);
        }
      }
    }
  }

  for (int i = 0; i < num_threads_to_remove; i++) {
    if (pthread_join(threads_to_remove[i], NULL) == 0) {
      logger("SUCCESS", "Thread has been terminated: ", &thread_name, 1);
    }
  }

  for (int i = 0; i < 100; i++) {
    if (thread_data[i].thread_id != 0) {
      for (int j = 0; j < num_threads_to_remove; j++) {
        if (thread_data[i].thread_id == threads_to_remove[j]) {
          thread_data[i].thread_id = 0;
          thread_data[i].thread_name[0] = '\0';
        }
      }
    }
  }
}

void list_processes() {
  while (1) {
    printf("List Processes Sub-Menu:\n");
    printf("1. Processes created through your code\n");
    printf("2. All processes on the computer\n");
    printf("3. Back to the main menu\n");
    printf("Select an option: ");

    char choice[256];
    if (fgets(choice, sizeof(choice), stdin)) {
      choice[strcspn(choice, "\n")] = '\0'; // Remove the newline character

      if (strcmp(choice, "1") == 0) {
        printf("Processes created through your code:\n");
        for (int i = 0; i < 100; i++) {
          if (process_data[i].pid != 0) {
            printf("Process ID: %lu, Name: %s\n",
                   (unsigned long)process_data[i].pid,
                   process_data[i].process_name);
          }
        }

      } else if (strcmp(choice, "2") == 0) {
        printf("All processes on the computer:\n");
        char *argv[] = {"ps", "-ef", NULL};
        char *const envp[] = {NULL};
        execvp("ps", argv);
        exit(1);
      } else if (strcmp(choice, "3") == 0) {
        printf("Returning to the main menu.\n");
        break;
      } else {
        printf("Invalid option. Try again.\n");
      }
    }
  }
}

void* producer(void* arg) {
    for (int i = 0; i < 10; i++) {
        char item[20];
        sprintf(item, "Item-%d", i);

        // Simulate work
        struct timespec wait_time;
        wait_time.tv_sec = 0;
        wait_time.tv_nsec = (int)(100000000 * (0.1 + (rand() / (double)RAND_MAX) * 0.4));
        nanosleep(&wait_time, NULL);

        sem_wait(&empty_sem);  // Wait for an empty slot
        sem_wait(&mutex_sem);  // Get exclusive access to the buffer

        // Add item to the buffer
        buffer[in] = strdup(item);
        in = (in + 1) % BUFFER_SIZE;

        printf("Produced %s. Buffer: [", item);
        for (int j = 0; j < BUFFER_SIZE; j++) {
            if (buffer[j] != NULL) {
                printf("%s, ", buffer[j]);
            }
        }
        printf("]\n");

        sem_post(&mutex);  // Release the mutex
        sem_post(&filled);  // Notify that a slot is filled
    }
    pthread_exit(NULL);
}

int main() {
  if (pipe(pipe_fd) == -1) {
    perror("Pipe creation failed");
    exit(1);
  }

  int sem_id;
  key_t key = ftok("/tmp", 'a');
  if ((sem_id = semget(key, 1, 0666 | IPC_CREAT)) == -1) {
    logger("ERROR", "Semaphore creation failed", 0, 2);
    exit(1);
  }

  if (pipe(pipe_fd) == -1) {
    perror("Pipe creation failed");
    exit(1);
  }

  create_thread("Thread1");
  create_thread("Thread2");

  sem_unlink("/mutex_sem");
  sem_unlink("/empty_sem");
  sem_unlink("/filled_sem");
  initialize_sems();
  if (empty_sem == SEM_FAILED) {
    perror("Failed to open semphore for empty");
    exit(-1);
  }

  char message[] = "Hello Pipes";
  ipc_send_message(message);

  char *recieved_message = ipc_recieve_message();
  if (recieved_message) {
    printf("Recived message: %s", recieved_message);
    free(recieved_message);
  }

  close(read_pipe);
  close(write_pipe);

  // Add more threads as needed
  // pthread_join() or other synchronization methods can be used to manage
  // thread execution
  return 0;
}
