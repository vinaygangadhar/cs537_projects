#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

/* Some common varibles and declarations */
enum { max_char = 514, max_args = 5 };

//Struct Command
struct command {
	char* type;
	char command[max_char];
	char* eargv[max_args];
	int opt_cnt;
	int cmd_flag;
	int back_f, redr_f;
};

/* Error Function for all the errors */
void error(int msg){
	//char error_message[30] = "An error has occurred\n";
	printf("An error has occured:%d", msg);
	//write(STDERR_FILENO, error_message, strlen(error_message));
	//exit(1);
}

/* Argument checker Function */
int arg_checker(int* argc_p){
//	printf("ARG CHECKER\n");
	if(*argc_p == 1)				return (0);		//Call the mysh terminal
	else if(*argc_p == 2)		return (1);		//Enter batch mode
	else										return (-1);	//If more arguments call error
}

/* MyShell Prompt */
struct command* sh_prompt(struct command* cmd)
{
	//printf("SHELL ENTER\n");
	char prompt[20] = "mysh> ";
	write(STDOUT_FILENO, prompt, strlen(prompt));
	
	fflush(stdout);
	//fgets reads 1 less that max_size chars from the stream - So it can now read max of 513 characters including \n
	fgets(cmd->command, max_char, stdin); 	

	//Command length checker
  unsigned int cmd_ln = ((unsigned) strlen(cmd->command)) - 1;
	//printf("Command length is %u\n", cmd_ln);
	if ((cmd_ln) > 512) error(9);

	return (cmd);
}


/* Command Tokenizer and Parser */
struct command* cmd_parse(struct command* cmd)
{
	//printf("PARSER\n");
	char* token;
	int cnt = 0;

//	printf ("Command entered is %s\n", cmd->command);
	char delims[] = " >&\t\r\n\v";
	
	//Check for special characters- if > or & present enable 2 flags
	if (strstr (cmd ->command , ">") != NULL)
		cmd->redr_f = 1;
	else 
		cmd->redr_f =0;
	if (strstr (cmd ->command , "&") != NULL)
		cmd->back_f = 1;
	else
		cmd->back_f =0;


	//Get the commands and store in exec_args
	token = strtok(cmd->command, delims);
	while(token != NULL)
	{
	//	printf("%s\n", token);
	 	cmd->eargv[cnt] = token;
		token = strtok(NULL, delims);
		cnt++;
	}
	cmd->eargv[cnt] = NULL;
	cmd->opt_cnt = cnt;

//	printf("Number of Tokens: %d\n", cmd->opt_cnt);
//	printf("Redirect = %d, Background = %d \n", cmd->redr_f, cmd->back_f);
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
				//printf("Entered command is %s and Found command is %s\nSetting cmd_flag = %d\n", cmd->type, cmd_args[n], cmd->cmd_flag);
			}
		
		}
	}
	
	return (cmd);
}

/* Process creation and execution */

int exec_cmd(struct command* cmd)
{
//	printf("PROCESS CREATION\n");
	int rc = fork();

	if(rc < 0)
		error(1);

	else if(rc == 0)
	{
//		printf("I'm the child process with PID = %d with rc = %d %d \n", (int) getpid(), rc,cmd -> redr_f);
		printf ("CAME HERE!! redirect =");

		if(cmd-> redr_f== 1)
		{
			int close_rc = close (STDOUT_FILENO);
			if(close_rc <0)
			{
				error(2);
			//	exit(0);
			}
			printf("%s", cmd -> eargv[2]);
			//	printf("CAME HERE!!!\n");
			int new_fd = open(cmd->eargv[2], O_RDWR| O_TRUNC| O_CREAT,S_IRWXU );
			if(new_fd <0)
			{
				error(3);
				//exit(0);
			}

		}

		if(strcmp(cmd->type, "pwd") == 0 )
		{
			struct command temp;
			temp = *cmd;
			temp.eargv[1] =NULL;
			execvp(temp.type, temp.eargv);
			printf("PWD: THIS SHOULD NOT APPEAR");
		}
		
		/*if(strcmp(cmd->type, "cd") == 0 )
		{
			char* home = getenv("HOME");
			//execvp("chdir", home);
			chdir(home);
			//printf("CD: THIS SHOULD NOT APPEAR");
		}*/

	}

	else
	{
	if(cmd->back_f ==0)	
		wait(NULL);
		//printf("I'm the parent process of child rc: %d with PID = %d and waited till %d finished\n", rc, (int) getpid(), wc);
	}

	return 0;
}


/* Shell Mode */
int shell_mode()
{
	//Call interactive mode shell
	
//	printf("SHELL MODE\n");
	struct command execcmd;
	struct command* cmd = &execcmd;

	//Print the shell prompt, accept the command and check for the length
	while(1){   //strcmp(cmd->type, "exit") != 0
		cmd = sh_prompt(cmd);

		//If command length is correct, tokenize and parse the commands
		cmd = cmd_parse(cmd);
		
		char* home = getenv("HOME");
		//If no command is found in exec_args[0] then wrong command is entered
		if(cmd->opt_cnt != 0){
			if(cmd->cmd_flag == 0)
				error(4);
			else if( strcmp(cmd->type, "exit") == 0)
				exit(0);
			else if (strcmp(cmd->type, "cd") == 0)
			{
				int derror =0;
				if(cmd->opt_cnt ==1)
					derror = chdir(home);
				else
					derror = chdir(cmd ->eargv[1]);
				if(derror <0)
					error(5);
			}
			else{
				//If command present, then parse the options
				int ret_e = exec_cmd(cmd);
				if (ret_e != 0)
					error(6);
			}
		}

	}

	return 0;
}

/* Batch Mode */
int batch_mode(char* batch_file)
{
	//printf("BATCH MODE\n");
	return 0;
}

//MAIN Program
int main (int argc, char* argv[]){
	
 	//Argumnets check
	int exec_flag = arg_checker(&argc);

	//Check foe execution mode
	if(exec_flag < 0)
		error(7);

	else if (exec_flag == 0) {
		//Call interactive mode shell
		//printf ("Entering shell mode\n");
		int ret_s = shell_mode();
		if (ret_s != 0)
			error(8);
	}

	else {
		//printf ("Entering batch mode\n");
		
		char* batch_file = argv[1];
		int ret_b = batch_mode(batch_file);
		if (ret_b != 0)
			error(9);
	}

	return 0;
}
