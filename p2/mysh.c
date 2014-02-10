#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

enum { max_char = 512 };
static char command[max_char];
static char* batch_file = "random_file";
static int flag;

//Error Function
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
void arg_check (int argc_p, char** argv_p ){
	
	if (argc_p == 1)
	{
		//Call the mysh terminal
		flag = 0;
		print_sh();
		fflush(stdout);
		fgets(command, max_char, stdin);
	}
	else if(argc_p == 2)
	{
		//Enter batch mode and check whether its the argument is a file
		flag = 1;
		batch_file = argv_p[1];
	}
	else
	{
		//If more arguments call error
		error();
	}

}


//MAIN Program
int main (int argc, char* argv[]){

 	//Argumnets check
	arg_check(argc, argv);

  //Check the flag and decide the execution mode
	int i = 0;
	char* token;
	char* exec_args[] = {};
 	//char* exec1 = NULL;

	if (flag == 0)
	{
		printf ("Command entered is %s\n", command);
		token = strtok(command," \n");
		while(token != NULL)
		{
			printf("%s\n", token);
			exec_args[i] = token;
			token = strtok(NULL," \n");
			i++;
		}
		exec_args[i] = NULL;
	}
	else 
		printf ("Entering batch mode and teh batch file is %s\n", batch_file);
 	//Do some file operations and parse the command for batch mode
 


	//Call child process - call fork
	int rc = fork();

	if (rc < 0) {
		error();
	}











	return 0;
}
