#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

enum { max_char = 512 };
static char command[max_char];


//Error Function
void error(){
	char error_message[30] = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
}

void print_sh(){
	printf("mysh> ");
}

//Arg check Function
void arg_check (int argc_p, char** argv_p ){
	char* batch_file = "";

	if (argc_p == 1)
	{
		//Call the mysh terminal
		print_sh();
		while(1){
			fflush(stdout);
			fgets(command, max_char, stdin);
			if (command != NULL) break;
		}
	}
	else if(argc_p == 2)
	{
		//Enter batch mode and check whether its the argument is a file
		printf("Enter batch mode\n");
		batch_file = argv_p[1];
	}
	else
	{
		error();
	}

}



//MAIN Program
int main (int argc, char* argv[]){

 	//Argumnets check
	arg_check(argc, argv);

	//Call child process - call fork
	int rc = fork();

	if (rc < 0) {
		error();
	}











	return 0;
}
