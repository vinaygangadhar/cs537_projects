#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>


/* Some common varibles and declarations */
enum { max_char = 514 };
static char* eargv[] = {};

/* Error Function for all the errors */
void error(){
	char error_message[30] = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
	exit(1);
}

/* MyShell Prompt */
void sh_prompt(){
	char prompt[20] = "mysh> ";
	write(STDOUT_FILENO, prompt, strlen(prompt));
}

/* Argument checker Function */
int arg_checker(int* argc_p){
	if(*argc_p == 1)				return (0);		//Call the mysh terminal
	else if(*argc_p == 2)		return (1);		//Enter batch mode
	else										return (-1);	//If more arguments call error
}

/* Command Length checker */
void cmd_checker(char* command){
	char* cmd = command;
  unsigned int cmd_ln = ((unsigned) strlen(cmd)) - 1;
//	printf("Command length is %u\n", cmd_ln);
	if ((cmd_ln) > 512) error();
}

/* Command Tokenizer and Parser */
int cmd_tp(char* command, int* opt_cnt){
	char* token;
	char* cmd = command;
	int cnt = *opt_cnt;

	printf ("Command entered is %s\n", cmd);
	char delims[] = " >&\t\r\n\v";

	//Get the commands and store in exec_args
	token = strtok(cmd, delims);
	while(token != NULL)
	{
//		printf("%s\n", token);
	 	eargv[cnt] = token;
		token = strtok(NULL, delims);
		cnt++;
	}
	eargv[cnt] = NULL;
	printf("Number of Tokens: %d\n", cnt);

	//Check for special characters
	
	// Parser //
	char cmd_args[][5] = { "exit", "pwd", "cd", "wait" };
	int n;
  int cmd_flag = 0;

	for(n = 0; n < 4; n++)
	{
		//Compare upto 4 characters
		if (strncmp (cmd_args[n], eargv[0], 4) == 0) 
		{
			printf("Entered command is %s and Found command is %s\n", eargv[0], cmd_args[n]);
			cmd_flag = 1;
		}
	}
	
	//If no command is found in exec_args[0] then wrong command is entered
	if (cmd_flag == 0) 
		error();

	return 0;
}

/* Process create */
int exec_cmd(){
	
	int rc = fork();

	if(rc < 0)
		error();

	else if(rc == 0){
		printf("I'm the child process with PID = %d with rc = %d\n", (int) getpid(), rc);

			if(strcmp(eargv[0], "exit") == 0){
				exit(0);
			}
			else if(strcmp(eargv[0], "pwd") == 0 ){
				char success[30] = "Executing pwd\n";
				write(STDOUT_FILENO,success, strlen(success));
				execvp(eargv[0], eargv);
			}
			else if(strcmp(eargv[0], "cd") == 0)
			{
			}
	}

	else{
	int wc = wait(NULL);
		printf("I'm the parent process of child %d: rc with PID = %d and waited till %d finished\n", rc, (int) getpid(), wc);
	}

	return 0;
}



/* Interactive Mode Shell */
int shell_mode(){
	char command[max_char];
		
	//Print the shell prompt
	sh_prompt();
	fflush(stdout);
	
	//fgets reads 1 less that max_size chars from the stream - So it can now read max of 513 characters including \n
	fgets(command, max_char, stdin); 		

	//Check for the command string length - Should be <=512 bytes
	cmd_checker(command);

	//If command length is correct, tokenize and parse the commands
	int opt_cnt = 0; //token count
	if(cmd_tp(command, &opt_cnt) != 0)
		error();

	//Use opt_cnt to check for all possible options
	
	//If command present, then parse the options
	exec_cmd();

	return 0;
}


/* Batch Mode Shell */
void batch_mode (char* batch_file){
	printf ("Entering batch mode and the batch file is %s\n", batch_file);
 	//Do some file operations and parse the command for batch mode
}


//MAIN Program
int main (int argc, char* argv[]){
	
 	//Argumnets check
	int exec_flag = arg_checker(&argc);
	
	if(exec_flag < 0)
		error();

	else if (exec_flag == 0) {
		//Call interactive mode shell
	int ret_s = shell_mode();
	if (ret_s != 0)
		error();
	}

	else {
		char* batch_file = argv[1];
		batch_mode(batch_file);
	}

	return 0;
}
