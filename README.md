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
