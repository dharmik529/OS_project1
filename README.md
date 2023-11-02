# Project 1 

## DUE : October 29, 2023

### Project Theme: Advanced Process Manager with Process Synchronization

**Overview**

**Required Functionalities for the process manager.**

1. Process Creation:

- Implement a process creation mechanism that allows users to create new processes.
- Use system calls (e.g., fork, exec) for process creation.

2. Process Management:

- Develop functionalities to list, terminate, and monitor running processes.
- Allow users to view information about each process, such as its process ID (PID), parent process ID, and state.

3. Thread Support:

- Extend the Process Manager to support multiple threads within a process.
- Implement thread creation, termination, and synchronization mechanisms.
- Use system calls for thread creation (e.g., pthread_create) and synchronization (e.g., mutexes, semaphores)

4. Inter-Process Communication (IPC):

- Implement IPC mechanisms to allow processes and threads to communicate and share data.
- Explore methods like message passing, shared memory, or pipes for IPC.
- Use system calls for IPC operations (e.g., pipe, msgget, shmget).

5. Process Synchronization:

- Implement synchronization primitives such as mutexes, and semaphores
- Demonstrate the use of synchronization mechanisms to solve common synchronization problems (e.g., producer-consumer, reader-writer).

6. Command-Line Interface (CLI) or Graphic User Interface (GUI) on your choice:

- Develop a user-friendly interface for interacting with the Process Manager.
- Allow users to create processes, create threads, synchronize threads, and perform IPC operations.
- Provide clear and informative command syntax and options.

7. Logging and Reporting:

- Implement logging and reporting features to track and display the execution of processes and threads.
- Log significant events, errors, and information related to process synchronization.

**Expected Skills Gained:**

- You will gain skills in manipulating processes and threads in many different aspects.
- You will have skills to exploit system calls to manipulate processes and threads to achieve your goal.
- You will have basic skill to develop a software which operates reliably with reducing conflicts in using system resources.

**Deliverables**
- **Source Code:** Source code including comments explaining the design and implementation as py file or c file loaded into github
- **Project Report:** The title of the project, a list of the implemented functionalities, description on how to install and how to use, the test result for each functionality with figure and explanation, and the discussion on the project result
- **Github Repository link:** The project report shall be given as readme file (Markdown file, readme.md) while the source code is loaded as a separate file 

**Your submission needs to be Github Repository link which contains the deliverables**

# Report

## List of Functions

```go
class ThreadInfo
class ProcessInfo
func logger
func empty_log_file
func ipc_send_message
func ipc_recieve_message
func initialize_sems
func thread_func
func create_thread
func list_threads
func process_menu
func create_process
func terminate_thread
func list_processes
func producer
func consumer
func run_producer_consumer_example
func main
```

## Installation

Ensure clang version is >15.0.0

Compile:
`clang main.c -o main.out`

Run the binary:
`./main.out`

## Process Creation

This test validates the Process Manager's ability to effectively generate and oversee new processes. It leverages the os.fork() function to create fresh processes, showcasing the system's concurrent processing capabilities.

```c
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
```

## Thread Creation

### Single Thread
This test confirms the Process Manager's proficiency in managing the creation of single threads. It utilizes the pthread_create function to generate threads, showcasing the system's adeptness in handling multi-threaded applications.

```c
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
  } else {
    logger("ERROR", "Failed to create thread", 0, 2);
  }
}
```

### MultiThread 

This test validates the Process Manager's capability to manage multiple threads within a process. It demonstrates how threads within a process can function independently, illustrating the concept of multi-threading.

```
Sun Oct 29 21:48:09 2023
 [SUCCESS]: Thread is running: Thread2
Sun Oct 29 21:49:17 2023
 [SUCCESS]: Thread is running: Thread1
Sun Oct 29 21:49:17 2023
```

## Thread Termination


This test ensures that the Process Manager can effectively oversee the termination of individual threads. It employs the pthread_cancel and pthread_join functions to request and confirm the termination of threads.

```c
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
```

## List Processes

This test guarantees the Process Manager's ability to accurately enumerate and furnish information about processes. It provides visibility into both processes initiated by the code and all processes on the computer. The test employs the psutil library to collect process-related data.

```c
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
```

## Inter-Process-Communication (IPC)

This test confirms the Process Manager's competence in facilitating inter-process communication using IPC (Inter-Process Communication). It utilizes named pipes (FIFOs) for message exchange between processes and employs non-blocking reads to receive messages. This test also confirms the Process Manager's capacity to receive messages sent by other processes via IPC. It employs non-blocking reads to handle the reception of IPC messages.

```c
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
    buffs[bytes_read] = '\0'; // Null-terminate the received message
    return strdup(buffs);     // Return a dynamically allocated string
  }
}
```

## Process Synchronization 

This test shows the Process Manager's proficiency in implementing process synchronization and orchestrating threads to address the traditional Producer-Consumer problem. It utilizes semaphores and mutexes for achieving synchronization.

```c
void *producer(void *arg) {
  for (int i = 0; i < 10; i++) {
    char item[20];
    sprintf(item, "Item-%d", i);

    // Simulate work
    struct timespec wait_time;
    wait_time.tv_sec = 0;
    wait_time.tv_nsec =
        (int)(100000000 * (0.1 + (rand() / (double)RAND_MAX) * 0.4));
    nanosleep(&wait_time, NULL);

    sem_wait(&empty_sem); // Wait for an empty slot
    sem_wait(&mutex_sem); // Get exclusive access to the buffer

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

    sem_post(&mutex_sem);  // Release the mutex
    sem_post(&filled_sem); // Notify that a slot is filled
  }
  pthread_exit(NULL);
}

void *consumer(void *arg) {
  for (int i = 0; i < 10; i++) {
    // Simulate work
    struct timespec wait_time;
    wait_time.tv_sec = 0;
    wait_time.tv_nsec =
        (int)(100000000 * (0.1 + (rand() / (double)RAND_MAX) * 0.4));
    nanosleep(&wait_time, NULL);

    sem_wait(&filled_sem); // Wait for a filled slot
    sem_wait(&mutex_sem);  // Get exclusive access to the buffer

    // Remove and consume the first item
    char *item = buffer[out];
    buffer[out] = NULL;
    out = (out + 1) % BUFFER_SIZE;

    printf("Consumed %s. Buffer: [", item);
    for (int j = 0; j < BUFFER_SIZE; j++) {
      if (buffer[j] != NULL) {
        printf("%s, ", buffer[j]);
      }
    }
    printf("]\n");

    free(item);

    sem_post(&mutex_sem); // Release the mutex
    sem_post(&empty_sem); // Notify that a slot is empty
  }
  pthread_exit(NULL);
}
```

## Logging


The Advanced Process Manager relies heavily on logging to capture vital information pertaining to processes, threads, IPC activities, and synchronization events.
The log file serves as an invaluable tool for analyzing the behavior of processes and threads, debugging issues, and comprehending the functionality of synchronization mechanisms within the Process Manager. It empowers users to retrospectively examine past activities, probe errors, and trace the sequence of events in both individual processes and multi-threaded contexts.

```c
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
```

## Conclusions

The Advanced Process Manager project is a robust and multifunctional system designed to manage processes and threads in a multi-threaded environment. This discussion will provide a comprehensive overview of the project's functionalities, design, and the broader implications of its capabilities, all condensed into a single page.

**Comprehensive Functionality**
The project offers a wide range of functionalities, enabling the creation of new processes, each with its dedicated menu for further interactions. This modularity in process creation facilitates the management of various tasks and services within the system. Users can access essential management options for processes, including process listing and termination, providing a high level of control over these entities.

**Thread Support**
One of the project's strengths is its robust support for threads within processes, significantly enhancing its functionality. This demonstrates the system's ability to handle multi-threaded applications, making it an ideal platform for more complex parallel computing tasks. The seamless implementation of thread creation and termination showcases the Process Manager's control over its threads.

**Inter-Process Communication (IPC)**
The inclusion of Inter-Process Communication (IPC) is a valuable addition to the project. IPC enables the exchange of messages between processes, emphasizing the system's capacity to coordinate actions and share information across different processes. This functionality is especially crucial in distributed and networked systems, highlighting the project's versatility.

**Process Synchronization**
The project introduces a notable example of process synchronization through the simulation of the Producer-Consumer problem. This practical exercise illustrates the Process Manager's capabilities in orchestrating complex thread interactions. Semaphore-based synchronization ensures that only one thread can access shared resources at any given time, addressing a fundamental requirement in parallel computing.

**Logging and Reporting**
To enhance accountability and visibility, the project features a comprehensive logging system that records all relevant information in the "processes.log" file. This log serves as a historical record of processes, threads, IPC activities, and synchronization events. It proves invaluable for debugging, performance analysis, and gaining insights into system behavior. The "Log Clearance" feature allows users to reset the log for new sessions, maintaining log integrity.

In summary, the Advanced Process Manager project is a sophisticated and multifaceted system that excels in managing processes and threads in a multi-threaded environment. Its comprehensive functionalities, support for threads, Inter-Process Communication, process synchronization, and robust logging system make it a valuable tool for a wide range of applications, from managing everyday tasks to complex parallel computing endeavors.
