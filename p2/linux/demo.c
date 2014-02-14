#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {

  // Call fork()
  int rc = fork();

  // fork() creates two copies of this process
  // Both processes continue execution from *this point*

  // Failure
  if (rc < 0) {
    fprintf(stderr, "fork() failed\n");
    exit(1);
  }

  // Child uses exec to run a different program
  else if (rc == 0) {
    printf("I'm the child process (PID = %d) with rc = %d\n", (int) getpid(), rc);

    // Prepare the args for execvp()
    // These are processed as if they were typed on the command line
    char* exec_args[4];

    // By convention, first arg is the name of the new file to execute
    exec_args[0] = "ls";

    // Other args
    exec_args[1] = "-l";
    exec_args[2] = "-a";

    // Last arg must be NULL pointer to terminate list
    exec_args[3] = NULL;
 
    // Before calling execvp(), redirect output to a file
   
    // Close the file descriptor associated with stdout
    int close_rc = close(STDOUT_FILENO);
    if (close_rc < 0) {
      perror("close");
      exit(1);
    }

    // Open a new file
    // This new file will be assigned the smallest available descriptor, which
    // will equal STDOUT_FILENO, which we made available using close()
    int fd = open("redirected_output.txt", O_RDWR | O_TRUNC | O_CREAT, S_IRWXU);
    if (fd < 0) {
      perror("open");
      exit(1);
    }

    printf("File descriptor number of new opened file = %d\n", fd);
    printf("File descriptor number of stdout = %d\n\n", STDOUT_FILENO);

    // Now, any output sent to stdout will go to the file
    // This includes any calls to print()

    // Call execvp() to change the process
    // First argument is name of program to run (same as exec_args[0])
    // Second argument is the pointer to the array of command line args
    execvp("ls", exec_args);

    // A sucessful call to execvp() never returns
    printf("If you're reading this, something went wrong.\n");
  }

  // Parent waits for child to finish
  else {
    
    // By default, wait() stops until any one child of this parent finishes
    int wc = wait(NULL);

    printf("I'm the parent of %d (PID = %d).  I waited until %d finished.\n", 
            rc, (int) getpid(), wc);
  }

  return(0);
} 
