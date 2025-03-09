#include "308sh.h"



int main(int argc, char* argv[]){
	int i;
	char* prompt;
	//Checks if prompt input is valid. If not valud default to 308sh
	if(argc != 3){
		printf("Using default prompt\n");
		prompt = "308sh";
	}else{
		prompt = argv[2];
	}
	//declare string object for command to be stored somewhere
	char command[100];
	//decalre working dir string with max length PATH_MAX constant
	char working_dir[PATH_MAX];
	int terminated = 0;
	while(!terminated){
		printf("%s>", 	prompt);
		//reads in command
		fgets(command, 100, stdin);
		//Removed trailing new line
		command[strcspn(command, "\n")] = 0;
		//Checks if command is exit which will then exit the shell
		int check_sys_val = check_system_command(command, working_dir);
		if(check_sys_val == 0){
			int status;
			int cpid = fork();
			if(cpid == 0){
				char* token = strtok(command, " ");
				int count = 1;
				while(token != NULL){
					printf("token in the first while: %s\n", token);
                    count++;
					token = strtok(NULL, " ");
				}
				char* arguements[count + 1];
				printf("The count: %d\n", count);	
				token = strtok(command, " ");
				arguements[0] = token;
				printf("Arguement 0: %s\n", arguements[0]);
				i = 1;
				while(token != NULL){
					printf("TOken in second While: %s\n", token);
					token = strtok(NULL, " ");
					arguements[i] = token;
					i++;	
				}
				arguements[i] = NULL;
				execvp("ls", arguements);
				exit(EXIT_SUCCESS);
			}else{
				waitpid(cpid, &status, 0);
				printf("I AM THE PARENT PROCESS I LIVE\n");
			}
		}else if (check_sys_val == -1){
			terminated = 1;
		}
	}
	return 0;
}

int check_system_command(char* command, char* working_dir){
		if(strcmp(command, "exit") == 0){
			return -1;
		}else if(strcmp(command, "pwd") == 0){
			//Print the current wd and store at same time
			printf("%s\n", getcwd(working_dir, PATH_MAX)); 
			return 1;
		}else if(strcmp(command, "cd") == 0){
			chdir(getenv("HOME"));
			getcwd(working_dir, PATH_MAX);
			return 1;
		}else{
			const char s[2] = " ";
			char* token = strtok(command, s);
			if(strcmp(token, "cd") == 0){
				token = strtok(NULL, s);
				if(token == NULL){
					chdir(getenv("HOME"));	
				}else{
					if(chdir(token) != 0){
						printf("No such file or directory \"%s\"\n", token);
						return 1;
					}
				}
				return 1;	
			}
			return 0;
		}
}	
