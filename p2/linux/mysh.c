#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

//#define DEBUG

/* Some common varibles and declarations */
enum { max_char = 514, max_args = 5 };

//Struct Command
struct command {
	char* type;
	char command[max_char];
	char* eargv[max_args];
	int opt_cnt;
	int cmd_flag;
	int redirect;
	char* redir_file;
	int bak;
};

/* Error Function for incorrect arguments */
void exit_error(){
	char error_message[30] = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
	exit(1);
}

/* Error Function for all the others errors */
void error(){
	char error_message[30] = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
}

/* Argument checker Function */
int arg_checker(int* argc_p){
	if(*argc_p == 1)				return (0);		//Call the mysh terminal
	else if(*argc_p == 2)		return (1);		//Enter batch mode
	else										return (-1);	//If more arguments call error
}

/* MyShell Prompt */
struct command* sh_prompt(struct command* cmd){
	char prompt[20] = "mysh> ";
	write(STDOUT_FILENO, prompt, strlen(prompt));
	
	fflush(stdout);
	//fgets reads 1 less that max_size chars from the stream - So it can now read max of 513 characters including \n
	fgets(cmd->command, max_char, stdin); 	

	//Command length checker
  unsigned int cmd_ln = ((unsigned) strlen(cmd->command)) - 1;
	
	#ifdef DEBUG
		printf("Command length is %u\n", cmd_ln);
	#endif

	if ((cmd_ln) > 512) error();
	return (cmd);
}


/* Command Tokenizer and Parser */
struct command* cmd_parse(struct command* cmd){
	char *token, *sub_char;
	int cnt = 0;

	printf ("Command entered is %s\n", cmd->command);

	char delims[] = " \t\r\n\v";
  char symbols[] = " >";

	//Get the commands and store in exec_args
	token = strtok(cmd->command, delims);
	while(token != NULL)
	{
			sub_char = strchr(token, '>');  //Parse for '>'
			
			if( (sub_char != NULL) && (*sub_char == '>') ){
					printf("Found > at: %d\n", sub_char-token);

					token[sub_char-token] = ' ';
					token = strtok(token, delims);
					printf("TOKEN Found: %s\n", token);
					cmd->eargv[cnt] = token;
					cnt++;
					
					token = strtok(NULL, delims);
					if(token != NULL){
							printf("Redirect File: %s\n", token);
							cmd->redirect = 1;
							cmd->redir_file = token;
					}
					else printf("redirect file not specified\n");
			}
			else{
					printf("TOKEN Found: %s\n", token);
					cmd->eargv[cnt] = token;
					token = strtok(NULL, delims);
					cnt++;
			}

	}

	cmd->eargv[cnt] = NULL;
	cmd->opt_cnt = cnt;

	printf("Number of Tokens: %d\n", cmd->opt_cnt);
	
	// Parser //
	if(cmd->opt_cnt != 0){
		char* cmd_args[4] = { "exit", "pwd", "cd", "wait" };
		int n;
		cmd->cmd_flag = 0;

		for(n = 0; n < 4; n++)
		{
			//printf("Comparing the entered command: %s with command: %s\n", cmd->eargv[0], cmd_args[n]);
		
			//Compare upto 4 characters
			if (strncmp (cmd_args[n], cmd->eargv[0], 4) == 0){
				cmd->type = cmd->eargv[0];
				cmd->cmd_flag = 1;
				printf("Entered command is %s and Found command is %s\nSetting cmd_flag = %d\n", cmd->type, cmd_args[n], cmd->cmd_flag);
			}
		
		}
	}

	return (cmd);
}

/* Process creation and execution */

int exec_cmd(struct command* runcmd){
	int cpid  = fork();

	if(cpid < 0)
		error();

	else if(cpid == 0){
		#ifdef DEBUG
			printf("I'm the child process with PID = %d with cpid = %d\n", (int) getpid(), cpid);
		#endif

		if(strcmp(runcmd->type, "pwd") == 0 ){
			execvp(runcmd->type, runcmd->eargv);
		}
	}

	else{
		int wc = wait(NULL);
		#ifdef DEBUG
			printf("I'm the parent process of child cpid: %d with PID = %d and waited till %d finished\n", cpid, (int) getpid(), wc);
		#endif	
	}

	return 0;
}


/* Shell Mode */
int shell_mode(){
  
	//Print the shell prompt, accept the command and check for the length
	while(1){ 
		
		struct command execcmd = {0};
		struct command* cmd = &execcmd;

		cmd = sh_prompt(cmd);

		//If command length is correct, tokenize and parse the commands
		cmd = cmd_parse(cmd);
		
		//If eargv[0] is NULL, then no command is entered and its just \n
		if(cmd->opt_cnt != 0){
			if(cmd->cmd_flag == 0){ 
				//If entered command is not found and matched, then wrong command is entered
				error();
			}
			else if( strcmp(cmd->type, "exit") == 0){
				exit(0);
			}
			else{
				int ret_e = exec_cmd(cmd);
				if(ret_e != 0) 
					error();
			}
		}

	}
	return 0;
}

/* Batch Mode */
int batch_mode(char* batch_file){
	return 0;
}

//MAIN Program
int main (int argc, char* argv[]){
	
 	//Argumnets check
	int exec_flag = arg_checker(&argc);

	//Check foe execution mode
	if(exec_flag < 0)
		exit_error();

	else if (exec_flag == 0) {
		#ifdef DEBUG
			printf ("Entering shell mode\n"); //Call interactive mode shell
		#endif
		
		int ret_s = shell_mode();
		if (ret_s != 0)
			error();
	}

	else {
		#ifdef DEBUG
			printf ("Entering batch mode\n");  //Call batch mode
		#endif

		char* batch_file = argv[1];
		int ret_b = batch_mode(batch_file);
		if (ret_b != 0)
			error();
	}

	return 0;
}
