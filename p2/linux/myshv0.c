#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

/* Some common varibles and declarations */
enum { max_char = 514 };
static char command[max_char];
static char* batch_file = "random_file";
static int flag;

/* Error Function for all the errors */
void error(){
	char error_message[30] = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
	exit(1);
}

//MyShell Prompt
void print_sh(){
	printf("mysh> ");
}

//Arg check Function
void arg_check (int* argc_p, char** argv_p ){
	
	if (*argc_p == 1)
	{
		//Call the mysh terminal
		flag = 0;
		print_sh();
		fflush(stdout);
		fgets(command, max_char, stdin); //fgets reads 1 less that max_size chars from the stream - So it can now read max of 513 characters including \n
	}
	else if(*argc_p == 2)
	{
		//Enter batch mode
		flag = 1;
		batch_file = argv_p[1];
	}
	else
	{
		//If more arguments call error
		error();
	}

}

//Command Length checker
void cmd_checker () {

  unsigned int cmd_ln = ((unsigned) strlen(command)) - 1;
	printf("Command length is %u\n", cmd_ln);
	if ((cmd_ln) > 512) error();
}

//MAIN Program
int main (int argc, char* argv[]){

 	//Argumnets check
	arg_check(&argc, argv);

	//Check for the command string length - Should be <=512 bytes
	cmd_checker();

  //Check the flag and decide the execution mode
	int opt_cnt = 0;
	char* token;
	char* exec_args[] = {};

	if (flag == 0)
	{
		//printf ("Command entered is %s\n", command);
		token = strtok(command," \n");
		while(token != NULL)
		{
		//	printf("%s\n", token);
			exec_args[opt_cnt] = token;
			token = strtok(NULL," \n");
			opt_cnt++;
		}
		exec_args[opt_cnt] = NULL; //Last arg must be NULL pointer to terminate
	}
	else 
		printf ("Entering batch mode and the batch file is %s\n", batch_file);
 	//Do some file operations and parse the command for batch mode



  



	/* ####### Command Parser ######### */
	char cmd_args[][5] = { "exit", "pwd", "cd", "wait" };
	int n;
  int cmd_flag = 0;

	for(n = 0; n < 4; n++)
	{
		if (strncmp (cmd_args[n], exec_args[0], 4) == 0) //Compare upto 4 characters
		{
			printf("Entered command is %s and Found command is %s\n", exec_args[0], cmd_args[n]);
			cmd_flag = 1;
		}
	}
	
	//If no command is found in exec_args[0] then wrong command is entered	
	if (cmd_flag == 0) error(); 

	


  /*######### Command Parser ########## */

	
	
	
	
	
	
	
	//Call child process - call fork to execute one command
	int rc = fork();

	if (rc < 0) {
		error();
	}

	else if(rc == 0)
	{
		printf("I'm  the child calling execvp with Process ID: %d\n", (int) getpid());
		//A successful call to execvp never returns
//		execvp(exec_args[0], exec_args);
	}

	else {
			int wc = wait(NULL);
			printf("I'm  the parent of %d  with Process ID: %d. I waited till my child process %d finished\n", rc, (int) getpid(), wc);
			//arg_check(argc, argv);
	}


	return 0;
}
