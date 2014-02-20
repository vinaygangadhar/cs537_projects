#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>

#define DEBUG

/* Some common varibles and declarations */
enum { max_char = 1000, max_args = 20 };

//Struct Command
struct command 
{
	char* type;
	char command[max_char];
	char* eargv[max_args];
	int opt_cnt;
	int cmd_flag;
	int err_flag;
	char* redir_file;
	int red_t;
	int bak_t;
};

/* Error Function for incorrect arguments */
void exit_error()
{
	char error_message[30] = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
	exit(1);
}

/* Error Function for all the others errors */
void error()
{
	char error_message[30] = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
}

/* Argument checker Function */
int arg_checker(int* argc_p)
{
	if(*argc_p == 1)				return (0);		//Call the mysh terminal
	else if(*argc_p == 2)		return (1);		//Enter batch mode
	else										return (-1);	//If more arguments call error
}

/*Command Length Checker*/
int cmdlen_check(struct command* cmd)
{
  unsigned int cmd_ln = ((unsigned) strlen(cmd->command)) - 1;
	
	#ifdef DEBUG
		printf("Command length is %u\n", cmd_ln);
	#endif

	if ((cmd_ln) > 512) return (-1);

	return 0;
}

/* MyShell Prompt */
struct command* sh_prompt(struct command* cmd)\
{
	char prompt[20] = "mysh> ";
	write(STDOUT_FILENO, prompt, strlen(prompt));
	
	fflush(stdout);
	//fgets reads 1 less that max_size chars from the stream - So it can now read max of 999 characters including \n
	char* fgets_r = fgets(cmd->command, max_char, stdin); 	
	if (fgets_r == NULL) exit(0);

	int cmdlen_ret = cmdlen_check(cmd);
	if(cmdlen_ret < 0) cmd->err_flag = 1;

	return (cmd);
}

/* Command Tokenizer and Parser */
struct command* cmd_parse(struct command* cmd)
{
	
	#ifdef DEBUG
		printf ("Command entered is %s\n", cmd->command);
	#endif

	//Get the special characters count
	char *sym1, *sym2;
	int cnt1 = 0, cnt2 = 0;

	sym1 = strchr(cmd->command, '>');
	while(sym1 != NULL){
		cnt1++;
		sym1 = strchr(sym1+1, '>');
	}
	if(cnt1 > 1) cmd->err_flag = 1;

	sym2 = strchr(cmd->command, '&');
	while(sym2 != NULL){
		cnt2++;
		sym2 = strchr(sym2+1, '&');
	}
	if(cnt2 > 1) cmd->err_flag = 1;
  else if(cnt2 == 1) cmd->bak_t = 1;	
	//> and & count parsing done

	#ifdef DEBUG
		printf("Count of >: %d, Count of &: %d\n", cnt1, cnt2);
	#endif
	
	/** Parse for all the corner cases and raise err_flag, Get the commands and store in eargv array **/
	char *str1, *str2, *token, *subtoken;
	int cnt = 0;
	char delims[] = " &\t\r\n\v";
	char syms[] = ">\t\r\n\v";
	char *saveptr1, *saveptr2;
	int j;

	for(j=1, str1 = cmd->command; ;str1=NULL)
	{
			token = strtok_r(str1, delims, &saveptr1);
		
			if(token == NULL)
			{
			
				#ifdef DEBUG
					printf("Exiting now from the command tokenising:)\n");
				#endif

				break;
			}

			#ifdef DEBUG
				printf("Parsed %s\n", token);
			#endif

			if(strncmp(token,">",1) == 0)
			{
					subtoken = strtok_r(str1, delims, &saveptr1);
					#ifdef DEBUG
						printf("Parsed subtoken: %s\n", subtoken);
					#endif
					if(subtoken == NULL)
					{

						subtoken = strtok_r(token, syms, &saveptr2);
						#ifdef DEBUG
							printf("Parsed subtoken2: %s\n", subtoken);
						#endif
						if(subtoken == NULL)
						{
					
							#ifdef DEBUG
								printf("No redirect file specified\n");
							#endif
							
							cmd->err_flag = 1;	
							break;
						}
						else
						{
							cmd->redir_file = subtoken;
							#ifdef DEBUG
								printf("Output file specified is %s\n", subtoken);
							#endif
							cmd->red_t = 1;
						}
					}
					else
					{
						cmd->redir_file = subtoken;
				
						#ifdef DEBUG
							printf("Output file specified is %s\n", subtoken);
						#endif
						cmd->red_t = 1;
					}

			}
			else if(strchr(token, '>') != NULL)
			{
					for(str2 = token; ;str2 = NULL)
					{
						subtoken = strtok_r(str2, syms, &saveptr2);
				
						#ifdef DEBUG
							printf("Parsed subtoken3: %s\n", subtoken);
						#endif
						
						if( (cmd->red_t == 1) && (subtoken == NULL))
						{
							subtoken = strtok_r(NULL, delims, &saveptr1);
							if(subtoken == NULL)
							{
								#ifdef DEBUG	
									printf("No redir file specified\n");
								#endif
						
							cmd->err_flag = 1;
							break;
							}
							else
							{

								#ifdef DEBUG	
									printf("Output1 file is specified: %s\n", subtoken);
								#endif
								cmd->redir_file = subtoken;
								break;
							}
						}
						else if(cmd->red_t == 1)
						{
				
							#ifdef DEBUG	
								printf("Output2 file is specified: %s\n", subtoken);
							#endif

							cmd->redir_file = subtoken;	
							break;
						}
						else
						{
							#ifdef DEBUG	
								printf("Token2 is %s\n", subtoken);
							#endif

								cmd->eargv[cnt] = subtoken;
								cmd->red_t = 1;
								cnt++;
						}
					}
			}
			else
			{
					if(cmd->red_t == 1)
					{
						cmd->err_flag = 1;
						break;
					}

					#ifdef DEBUG
						printf("Token1 is %s\n", token);
					#endif
					cmd->eargv[cnt] = token;
					cnt++;
			}
	}

	cmd->eargv[cnt] = NULL;
	cmd->opt_cnt = cnt;

	#ifdef DEBUG
	int test;
	printf("Tokenized command is: ");
		
	for(test = 0; test < cmd->opt_cnt; test++)
			printf("%d-%s ", test, cmd->eargv[test]);

		printf("\n");
		printf("Number of Tokens: %d\n", cmd->opt_cnt);
	#endif

	//Built-in command Parser //
	if(cmd->opt_cnt != 0)
	{
			char* cmd_args[4] = { "exit", "pwd", "cd", "wait" };
			int n;

			for(n = 0; n < 4; n++)
			{
				#ifdef DEBUG
					printf("Comparing the entered command: %s with command: %s\n", cmd->eargv[0], cmd_args[n]);
				#endif

				//Compare upto 4 characters
				if (strncmp (cmd_args[n], cmd->eargv[0], 4) == 0)
				{
					cmd->type = cmd->eargv[0];
					cmd->cmd_flag = 1;
					
					#ifdef DEBUG
						printf("Entered command is %s and Found command is %s\nSetting cmd_flag = %d\n", cmd->type, cmd_args[n], cmd->cmd_flag);
					#endif
				}
			}

			if( strstr(cmd->eargv[0], ".py") != NULL){
					cmd->type = ".py";
					cmd->cmd_flag = 1;

					#ifdef DEBUG
						printf("Entered command is %s and Found command is %s\nSetting cmd_flag = %d\n", cmd->eargv[0], cmd->type , cmd->cmd_flag);
					#endif
			}
	}	
	
	return (cmd);
}

/* Process creation and execution */
int exec_cmd(struct command* cmd)
{
	int cpid  = fork();
	
	if(cpid < 0)
		error();

	else if(cpid == 0)
	{
			#ifdef DEBUG
				printf("I'm the child process with PID = %d with cpid = %d\n", (int) getpid(), cpid);
			#endif
		
			if(cmd->cmd_flag !=0)
			{	
				// Command pwd	
				if(strcmp(cmd->type, "pwd") == 0 )
				{
		
					struct command temp;
					temp = *cmd;

					#ifdef DEBUG
						printf("Command params = Type : %s, opt_cnt: %d, cmd_flag: %d, err_flag: %d, red_t: %d, bak_t: %d\n", temp.type, temp.opt_cnt, temp.cmd_flag, temp.err_flag, temp.red_t, temp.bak_t);
					#endif
				
					//badpwd case
					if (temp.opt_cnt > 1)
					{
						error();
						exit(1);
					}
					//File redirection	
					if(temp.red_t == 1)
					{
						int close_rc = close (STDOUT_FILENO);
						if(close_rc < 0)
						{
							#ifdef DEBUG
								char close_message[30] = "Cannot close the STDOUT\n";
								write(STDERR_FILENO, close_message, strlen(close_message));
							#endif
							error();
							exit(1);
						}

						int new_fd = open(temp.redir_file, O_RDWR | O_TRUNC | O_CREAT, S_IRWXU);
						if(new_fd < 0)
						{
							#ifdef DEBUG
								char open_message[30] = "Cannot open the file\n";
								write(STDERR_FILENO, open_message, strlen(open_message));
							#endif
							error();
							exit(1);
						}
					}
						
					//Use getcwd implmentation
					long size;
					char* buf;
					char* ptr;

					size = pathconf(".", _PC_PATH_MAX);

					if ((buf = (char *)malloc((size_t)size)) != NULL)
					{
						ptr = getcwd(buf, (size_t)size);
						ptr = strcat(ptr, "\n");
						write(STDOUT_FILENO, ptr, strlen(ptr));
						exit(0);
					}
					else
					{
						error();
						exit(1);
					}

					free(buf);
					//int exec_ret = execvp(temp.type, temp.eargv);
					//if(exec_ret == -1)
				}

				//Python interpreter
				else if(strcmp(cmd->type, ".py") == 0 )
				{
		
					struct command temp;
					temp = *cmd;
					int i;
					char* py_args[temp.opt_cnt + 2];
					py_args[0] = "";

					#ifdef DEBUG
						printf("Command params- Type : %s, opt_cnt: %d, cmd_flag: %d, err_flag: %d, red_t: %d, bak_t: %d\n", temp.type, temp.opt_cnt, temp.cmd_flag, temp.err_flag, temp.red_t, temp.bak_t);
					#endif
					
					//File redirection	
					if(temp.red_t == 1)
					{
						int close_rc = close (STDOUT_FILENO);
						if(close_rc < 0)
						{
							#ifdef DEBUG
								char close_message[30] = "Cannot close the STDOUT\n";
								write(STDERR_FILENO, close_message, strlen(close_message));
							#endif
							error();
							exit(1);
						}

						int new_fd = open(temp.redir_file, O_RDWR | O_TRUNC | O_CREAT, S_IRWXU);
						if(new_fd < 0)
						{
							#ifdef DEBUG
								char open_message[30] = "Cannot open the file\n";
								write(STDERR_FILENO, open_message, strlen(open_message));
							#endif
								error();
								exit(1);
						}
					}

					for(i = 0; i < temp.opt_cnt; i++)
					{
						py_args[i+1] = temp.eargv[i];
					}
					py_args[temp.opt_cnt + 1]= NULL;
			
					execvp("/usr/bin/python", py_args);
				}	
			}

			//Other non-builtin shell commands
			else                
			{
				struct command temp;
				temp = *cmd;

				if(temp.red_t == 1)
				{
					int close_rc = close (STDOUT_FILENO);
					if(close_rc < 0)
					{
						#ifdef DEBUG
							char close_message[30] = "Cannot close the STDOUT\n";
							write(STDERR_FILENO, close_message, strlen(close_message));
						#endif
						error();
						exit(1);
					}

					int new_fd = open(temp.redir_file, O_RDWR | O_TRUNC | O_CREAT, S_IRWXU);
					if(new_fd < 0)
					{
						#ifdef DEBUG
							char open_message[30] = "Cannot open the file\n";
							write(STDERR_FILENO, open_message, strlen(open_message));
						#endif
						error();
						exit(1);
					}
				}

				if( execvp(temp.eargv[0], temp.eargv) < 0 )
				{
					error();
					exit(1);
				}
			}
	}

	//Parent Process
	else
	{
		int status;
		
		if(cmd->bak_t != 1)
		{
			int ws = wait(&status);
			if (ws == -1)
			{
				return (-1);
			}
			else if(WIFEXITED(status))        
			{
					//did the child terminate normally?
					#ifdef DEBUG
						printf("Child process %ld exited with return code %d and status %d\n",(long)cpid, WEXITSTATUS(status), status);
						printf("I'm the parent process %ld\n", (long)getpid());
					#endif
			}
		}			
		else
		{
			#ifdef DEBUG
				printf("Child process %ld is running in background  with return code %d and status %d\n",(long)cpid, WEXITSTATUS(status), status);
				printf("I'm the parent process %ld\n", (long)getpid());
			#endif
			cmd->bak_t = 0;
		}

	}
	
	return 0;
}


int shell_mode(struct command* cmd)
{
		//Tokenize and parse the commands
		cmd = cmd_parse(cmd);
		
		//If eargv[0] is NULL, then no command is entered and its just \n
		if(cmd->opt_cnt != 0)
		{
				if(cmd->err_flag == 1)
				{
					return (-1);
				}

			  //For non built-in commands
				else if(cmd->cmd_flag == 0)
				{
					#ifdef DEBUG
						printf("Entering execution\n");
					#endif
				
					int ret_sb = exec_cmd(cmd);
					if(ret_sb != 0)
					{	
						return (-1);
					}
				}
				
				//If the command is a built-in one
				else
				{
						if( strcmp(cmd->type, "exit") == 0)
						{
							if(cmd->opt_cnt > 1)
							{
								return(-1);
							}
							else	exit(0);
						}
						else if( strcmp(cmd->type, "cd") == 0)
						{	
							struct command temp;
							temp = *cmd;

							#ifdef DEBUG
								printf("Command params- Type : %s, opt_cnt: %d, cmd_flag: %d, err_flag: %d, red_t: %d, bak_t: %d\n", temp.type, temp.opt_cnt, temp.cmd_flag, temp.err_flag, temp.red_t, temp.bak_t);
							#endif
			
							if(temp.red_t == 1)
							{
								int close_rc = close (STDOUT_FILENO);
								if(close_rc < 0)
								{
									#ifdef DEBUG
										printf("Cannot close the STDOUT\n");
									#endif
									return (-1);
								}	

								int new_fd = open(temp.redir_file, O_RDWR | O_TRUNC | O_CREAT, S_IRWXU);
								if(new_fd < 0)
								{
									#ifdef DEBUG
										printf("Cannot Open the file: %s\n", temp.redir_file);
									#endif
									return(-1);
								}
							}
							
							//If only cd is typed
							if(temp.opt_cnt == 1)
							{
								char* home = getenv("HOME");
								int ret_c =	chdir(home);
								if(ret_c < 0) 
								{
									#ifdef DEBUG
										printf("Cannot execute chdir()\n");
									#endif
									return (-1);
								}
							}
							else
							{
								int ret_c = chdir(temp.eargv[1]);
								if(ret_c < 0)
								{
									#ifdef DEBUG
										printf("Cannot execute chdir()\n");
									#endif
									return (-1);
								}
							}
						}

						//Command wait
						else if( strcmp(cmd->type, "wait") == 0)
						{	
							struct command temp;
							temp = *cmd;

							#ifdef DEBUG
								printf("Command params- Type : %s, opt_cnt: %d, cmd_flag: %d, err_flag: %d, red_t: %d, bak_t: %d\n", temp.type, temp.opt_cnt, temp.cmd_flag, temp.err_flag, temp.red_t, temp.bak_t);
							#endif
		
							//badwait case
							if (temp.opt_cnt > 1)
							{
								error();
								exit(0);
							}

							if(temp.red_t == 1)
							{
								int close_rc = close (STDOUT_FILENO);
								if(close_rc < 0)
								{
									#ifdef DEBUG
										printf("Cannot close the STDOUT\n");
									#endif
									return (-1);
								}	

								int new_fd = open(temp.redir_file, O_RDWR | O_TRUNC | O_CREAT, S_IRWXU);
								if(new_fd < 0)
								{
									#ifdef DEBUG
										printf("Cannot Open the file: %s\n", temp.redir_file);
									#endif
									return(-1);
								}
							}
							
							//wait for all the background jobs to complete
							waitpid(-1, NULL , 0);
						}

						//Call Exec_cmd
						else
						{
							#ifdef DEBUG
								printf("Entering execution\n");
							#endif
							int ret_e = exec_cmd(cmd);
							if(ret_e != 0)
							{
								return (-1);
							}
						}		
					}
				}

		return 0;
}


/* Batch Mode */
int batch_mode(char* batch_file)
{
	char* b_file = batch_file;

	FILE* bf = fopen(b_file, "r");
	if(bf == NULL)
	{
		return(-1);
	}
	else
	{
		char* line = NULL;
		size_t len = 0;
		long int read;
		
		while ((read = getline(&line, &len, bf)) != -1)
		{
			struct command batch_cmd = {0};
			struct command* cmd = &batch_cmd;
		
			//printf("Retrieved line of length %ld :\n", read);
			//printf("%s", line);
			
			if(strcpy(cmd->command,line) == NULL)
				cmd->err_flag = 1;
			
			write(STDOUT_FILENO, cmd->command, strlen(cmd->command));

			int ret_c = cmdlen_check(cmd);
			if(ret_c < 0)
			{
				cmd->err_flag = 1;
			}

			//Call the shell_mode for each command
			int ret_s = shell_mode(cmd);
			if (ret_s != 0)
				error();
		}

		free(line);
	}
	
	fclose(bf);
	return 0;
}



//MAIN Program
int main (int argc, char* argv[]){
	
 	//Argumnets check
	int exec_flag = arg_checker(&argc);

	//Check foe execution mode
	if(exec_flag < 0)
		exit_error();

	else if (exec_flag == 0)
	{
		#ifdef DEBUG
			printf ("Entering shell mode\n"); //Call interactive mode shell
		#endif
		
		while(1)
		{

			struct command execcmd = {0};
			struct command* cmd = &execcmd;
			
			//Call the prompt intearctively
			cmd = sh_prompt(cmd);
			
			//Then call teh shell_mode
			int ret_s = shell_mode(cmd);
			if (ret_s != 0)
				error();
		}
	}

	else
	{
		#ifdef DEBUG
			printf ("Entering batch mode\n");  //Call batch mode
		#endif

		char* batch_file = argv[1];
		int ret_b = batch_mode(batch_file);
		if (ret_b != 0)
		{
			error();
			return (1);
		}
	}

	return 0;
}
