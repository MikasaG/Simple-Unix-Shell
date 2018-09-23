/* Name : Chang Gong
   V number : V00898803*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_INPUT_LINE 150
#define MAX_PROMPT_LINE 11
#define MAX_PARTS_LINE 100
#define MAX_NUM_TOKENS 9
#define MAX_NUM_PARTS 4
#define MAX_NUM_DIRECTORIES 10
#define MAX_DIRECTORY_LINE 80

typedef struct command_info command;
struct command_info {// define a struct, to store all information from the user input.
    char *cmd[3][9];
    char file[30];
    int type; //0-normal, 1-OR, 2-PP
};

command get_cmd(char* input);// function prototype.

command get_cmd(char* input) {// a specific function to get user's input , return the struct "command"
    char buff[3] = {0};
    fgets(input, MAX_INPUT_LINE, stdin);
    if (input[strlen(input) - 1] == '\n') {
        input[strlen(input) - 1] = '\0';
    }
    if(strcmp(input,"exit") == 0) {
	exit(0);
    }
    strncpy(buff,input,3);
    buff[2] = '\0';// buff copied the first two character of the user input. check if its = OR or PP.
    command e = {.type = -1,.file = {0}};
    if (strncmp(buff,"OR",2) == 0) { // if the command start with "OR"
	char *partition[MAX_NUM_PARTS] = { NULL };
        int n = 0;
        char *a = NULL;
        a=strstr(input,"->");
        while(a!=NULL) {// a simple loop to compute how many times "->" appears.
            n++;
            a=strstr(a+strlen("->"),"->");
        }
        char *p;
        int  num_parts = 0;
        p = strtok(input + 3, ">");
        while (p != NULL && num_parts< MAX_NUM_PARTS) { // split the input to different partition , according to "->"
                partition[num_parts] = p;
                num_parts++;
                p = strtok(NULL, ">");
        }
        partition[0][strlen(partition[0]) - 1] = '\0';
        if (num_parts != n+1 || partition[0] == NULL || partition[1] == NULL || n != 1 || num_parts != 2) { // here is some situation when the syntax is wrong.
            return e;
        } else {
            char *token[MAX_NUM_TOKENS] = { NULL };
            char *t = NULL;
            int  num_tokens = 0;
            t = strtok(partition[0], " ");
            while (t != NULL && num_tokens < MAX_NUM_TOKENS) { // split command and its option/argument.
                token[num_tokens] = t;
                num_tokens++;
                t = strtok(NULL, " ");
            }
            command a = {.type = 1,.file = {0}};
            for (int i = 0; i < MAX_NUM_TOKENS; i++) {
                if (i < num_tokens) {
                    a.cmd[0][i] = token[i]; // the first item in each cmd array is always the command, followed by its option/arguments.
                } else {
                    a.cmd[0][i] = NULL;
                }
            }
            strncpy(a.file,partition[1] + 1,strlen(partition[1]) + 1);
	    if (a.cmd[0][0] == NULL) {return e;} else {return a;}
        }
    } else if (strncmp(buff,"PP",2) == 0) { // if the command start with "PP"
        char *partition[3] = {NULL};
        int n = 0;
        char *a = NULL;
        a=strstr(input,"->");
        while(a!=NULL) {// a simple loop to compute how many times "->" appears.
            n++;
            a=strstr(a+strlen("->"),"->");
        }
        char *p;
        int  num_parts = 0;
        p = strtok(input + 3, ">");
        while (p != NULL && num_parts< MAX_NUM_PARTS) { // split the input to different partition , according to "->"
                partition[num_parts] = p;
                num_parts++;
                p = strtok(NULL, ">");
        }
        partition[0][strlen(partition[0]) - 1] = '\0';
	if (num_parts == 3) {
        	partition[1][strlen(partition[1]) - 1] = '\0';
	}
        if(n < 1 || n > 2 || num_parts != n+1 || partition[0] == NULL || partition[1] == NULL || (n == 2 && partition[2] == NULL) ) { // here is some situation when the syntax is wrong.
            return e;
        } else {
            command r = {.type = 2,.file = {0}};
            for (int i = 0; partition[i] != NULL && i < num_parts; i++) {
                char *token[MAX_NUM_TOKENS] = { NULL };
                char *t;
                int  num_tokens = 0;
                t = strtok(partition[i], " ");
            while (t != NULL && num_tokens < MAX_NUM_TOKENS){ // split command and its option/argument.
                    token[num_tokens] = t;
                    num_tokens++;
                    t = strtok(NULL, " ");
                }
                if (num_tokens == 0) {
                    return e;
                } else {
                    for (int j = 0; j < MAX_NUM_TOKENS; j++) {
                        if (j < num_tokens){
                            r.cmd[i][j] = token[j]; // the first item in each cmd array is always the command, followed by its option/arguments.
                        } else {
                            r.cmd[i][j] = NULL;
                        }
                    }
                }
            }
            if (r.cmd[0][0] == NULL || r.cmd[1][0] == NULL) {return e;} else {return r;}
        }
    } else { // if the command start without "PP" or "OR"
        int n = 0;
        char *a = NULL;
        a=strstr(input,"->");
        while(a!=NULL) { // a simple loop to compute how many times "->" appears.
            n++;
            a=strstr(a+strlen("->"),"->");
        }
        if(n > 0) {
            return e;
        } else {
            char *token[MAX_NUM_TOKENS];
            char *t = NULL;
            int  num_tokens = 0;
            t = strtok(input, " ");
            while (t != NULL && num_tokens < MAX_NUM_TOKENS) { // split command and its option/argument.
                token[num_tokens] = t;
                num_tokens++;
                t = strtok(NULL, " ");
            }
            command a = {.type = 0,.file = {0}};
            for (int i = 0; i < MAX_NUM_TOKENS; i++) {
                if (i < num_tokens) {
                    a.cmd[0][i] = token[i]; // the first item in each cmd array is always the command, followed by its option/arguments
                } else {
                    a.cmd[0][i] = NULL;
                }
            }
            return a;
        }

    }
}
int main(int argc, char *argv[]) {
    char prompt[MAX_PROMPT_LINE] = {0};
    char directories[MAX_NUM_DIRECTORIES][MAX_DIRECTORY_LINE] = {0};
    char inp[MAX_DIRECTORY_LINE] = {0};
    FILE* sh360rc = fopen(".sh360rc","r");
    if (sh360rc == NULL) {
    	fprintf(stderr, "cannot open .sh360rc for writing\n");
        exit(1);
    }
    fgets(prompt, MAX_PROMPT_LINE, sh360rc); //read the prompt
    if (prompt[strlen(prompt) - 1] == '\n') {
        prompt[strlen(prompt) - 1] = '\0';
    }
    int dir_n = 0;
    while (fgets(inp, MAX_DIRECTORY_LINE, sh360rc)) { // read each directory
	inp[strlen(inp) - 1] = '/';
	inp[strlen(inp)] = '\0';
	strncpy(directories[dir_n], inp, MAX_DIRECTORY_LINE);
	dir_n++;
    }
    fclose(sh360rc);
    for(;;) {
	char input[MAX_INPUT_LINE] = {0};
        fprintf(stdout,"%s", prompt); // print prompt
        fflush(stdout);
        command r = get_cmd(input); // get command from user
        if (r.type == -1) { // if the type of command is -1 ,means syntax wrong.
            fprintf(stderr, "The syntax of the command might be wrong\n");
        } else if (r.type == 0 || r.type == 1) { // if the type is 0 or 1, means there is only one command, no use of pipe.
            int dir_nums = 0;
            int find = 0;
	    char* path;
            while (dir_nums < dir_n)  {// a loop to check each directory, if the command exist, set find to 1, otherwise, find remains 0.
		char path_temp[MAX_DIRECTORY_LINE] = {0};
		strncpy(path_temp, directories[dir_nums], strlen(directories[dir_nums]));
                struct stat buf;
                strcat(path_temp,r.cmd[0][0]);
                if (stat(path_temp,&buf) == 0) {
		    path = path_temp;
                    find = 1;
                    break;
                }
                dir_nums++;
            }
            if (find == 1) { // if find the command
                if (r.type == 0) { // no need of output redirection,
                    char *envp[] = { 0 };
                    int pid;
                    int status;
                    if ((pid = fork()) == 0) {
                        execve(path, r.cmd[0], envp);
			printf("child: SHOULDN'T BE HERE.\n");
                    }                
                    waitpid(pid, &status, 0);
                } else if (r.type == 1) { // need output redirection,.
                    char *envp[] = { 0 };
                    int pid, fd;
                    int status;
                    if ((pid = fork()) == 0) {
                        fd = open(r.file, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
                        if (fd == -1) {
                            fprintf(stderr, "cannot open target file for writing\n");
                            exit(1);
                        }
                        dup2(fd, 1); // redirec the output
                        execve(path, r.cmd[0], envp);
                        printf("child: SHOULDN'T BE HERE.\n");
                    }
                    waitpid(pid, &status, 0);
                }
            } else {// if cannot find the command
		if (strcmp(r.cmd[0][0], "cd") == 0) { // if this is the command "cd"
		    if(chdir(r.cmd[0][1]) == -1) {
			printf("cd: %s: no such directory.\n",r.cmd[0][1]);
		    }
		} else if (strcmp(r.cmd[0][0], "help") == 0){ // if this is the command "help"
		    printf("\n\n\n\t*****************************************************************\n");
		    printf("\t			Welcome to sh360plus			\n");
		    printf("\t-----------------------------------------------------------------\n");
		    printf("\tWelcome to sh360plus, a basic shell program.\n");
		    printf("\tHow to use output redirection and pipe are described below:\n\n");
		    printf("\tOR cmd [-o] -> targetfile :	output redirection\n");
		    printf("\tPP cmd1 [-o] -> cmd2 [-o] -> cmd3 [-o]	    : pipe functionality\n\n");
		    printf("\tcd	    : change the shell working directory\n");
		    printf("\thelp	    : display information about this shell\n");
		    printf("\texit	    : close this shell\n");
		    printf("\tYou can not use output redirection and pipe at same time.\n");
		    printf("\t*****************************************************************\n\n\n");
		} else {
		    fprintf(stderr, "The command %s doesn't exist\n",r.cmd[0][0]);
		}        
            }
        } else if (r.type == 2) { // if type = 2, means we need pipe.
            int dir_nums = 0;
            if (r.cmd[2][0] == NULL) { // if we have 2 command.
                int find_cmd1 = 0;
                int find_cmd2 = 0;
		char path_cmd1[MAX_DIRECTORY_LINE] = {0};
                char path_cmd2[MAX_DIRECTORY_LINE] = {0};
                while (dir_nums < dir_n) { // a loop to check each directory, to see whether both commands exist.
		    struct stat buf;                    		   
                    if (find_cmd1 == 0) {
			strncpy(path_cmd1,directories[dir_nums],strlen(directories[dir_nums]));
			strcat(path_cmd1,r.cmd[0][0]);
			if (stat(path_cmd1,&buf) == 0) {
			    find_cmd1 = 1;
			    if (find_cmd2 == 1) {break;}
			}                 
                    }
		    if (find_cmd2 == 0) {
			strncpy(path_cmd2,directories[dir_nums],strlen(directories[dir_nums]));		    
		    	strcat(path_cmd2,r.cmd[1][0]);
			if (stat(path_cmd2,&buf) == 0) {
			    find_cmd2 = 1;
			    if (find_cmd1 == 1) {break;}
			}                 
                    }
                    dir_nums++;                  
                }
                if (find_cmd1 == 1 && find_cmd2 == 1) { // if we found both two commands.
                    char *envp[] = { 0 };
                    int status;
                    int pid_1, pid_2;
                    int fd[2];
                    pipe(fd);// estabilish a pipe between two fd.
                    if ((pid_1 = fork()) == 0) {
                        close(fd[0]); // the first child process doesn't need this 
                        dup2(fd[1], 1);
                        close(fd[1]); // dont need it anymore.
                        execve(path_cmd1, r.cmd[0], envp);
                        printf("child (head): SHOULDN'T BE HERE.\n");
                    }
                    close(fd[1]); // the second process  doesn't need fd[1]
                    if ((pid_2 = fork()) == 0) {
                        close(fd[1]);
                        dup2(fd[0], 0);
                        close(fd[0]);// dont need it anymore.
                        execve(path_cmd2, r.cmd[1], envp);
                        printf("child (tail): SHOULDN'T BE HERE.\n");
                    }
                    close(fd[0]);
                    waitpid(pid_1, &status, 0);
                    waitpid(pid_2, &status, 0);
                }else { // if we cannot find both commands.
                    fprintf(stderr, "The command %s or %s doesn't exist\n",r.cmd[0][0], r.cmd[1][0]);
                }
            } else { // if we have 3 command.
		int find_cmd1 = 0;
                int find_cmd2 = 0;
		int find_cmd3 = 0;
		char path_cmd1[MAX_DIRECTORY_LINE] = {0};
                char path_cmd2[MAX_DIRECTORY_LINE] = {0};
		char path_cmd3[MAX_DIRECTORY_LINE] = {0};
                while (dir_nums < dir_n) { // a loop to check each directory, to see whether all commands exist.
		    struct stat buf;                    		   
                    if (find_cmd1 == 0) {
			strncpy(path_cmd1,directories[dir_nums],strlen(directories[dir_nums]));
			strcat(path_cmd1,r.cmd[0][0]);
			if (stat(path_cmd1,&buf) == 0) {
			    find_cmd1 = 1;
			    if (find_cmd2 == 1 && find_cmd3 == 1) {break;}
			}                 
                    }
		    if (find_cmd2 == 0) {
			strncpy(path_cmd2,directories[dir_nums],strlen(directories[dir_nums]));		    
		    	strcat(path_cmd2,r.cmd[1][0]);
			if (stat(path_cmd2,&buf) == 0) {
			    find_cmd2 = 1;
			    if (find_cmd1 == 1 && find_cmd3 == 1) {break;}
			}                 
                    }
		    if (find_cmd3 == 0) {
			strncpy(path_cmd3,directories[dir_nums],strlen(directories[dir_nums]));		    
		    	strcat(path_cmd3,r.cmd[2][0]);
			if (stat(path_cmd3,&buf) == 0) {
			    find_cmd3 = 1;
			    if (find_cmd1 == 1 && find_cmd2 == 1) {break;}
			}                 
                    }
                    dir_nums++;          
                }
                if (find_cmd1 == 1 && find_cmd2 == 1 && find_cmd3 == 1) { // if we found all commands
                    char *envp[] = { 0 };
                    int status;
                    int pid_1, pid_2, pid_3;
                    int pipe_A[2]; //establish two pipe, pipe_A is used for process 1 and process 2.
                    int pipe_B[2]; //pipe_B is used for process 2 and process 3.
                    pipe(pipe_A);
                    if ((pid_1 = fork()) == 0) {
                        close(pipe_A[0]);
                        dup2(pipe_A[1], 1);
                        close(pipe_A[1]);
                        execve(path_cmd1, r.cmd[0], envp);
                        printf("child (1): SHOULDN'T BE HERE.\n");
                    }
                    close(pipe_A[1]);
                    pipe(pipe_B);
                    if ((pid_2 = fork()) == 0) {
                        close(pipe_B[0]);
                        dup2(pipe_A[0], 0);
                        dup2(pipe_B[1], 1);
                        close(pipe_A[0]);
                        close(pipe_B[1]);
                        execve(path_cmd2, r.cmd[1], envp);
                        printf("child (2): SHOULDN'T BE HERE.\n");
                    }
                    close(pipe_A[0]);
                    close(pipe_B[1]);
                    if ((pid_3 = fork()) == 0) {
                        dup2(pipe_B[0], 0);
                        close(pipe_B[0]);
                        execve(path_cmd3, r.cmd[2], envp);
                        printf("child (3): SHOULDN'T BE HERE.\n");
                    }
                    close(pipe_B[0]);
                    waitpid(pid_1, &status, 0);
                    waitpid(pid_2, &status, 0);
                    waitpid(pid_3, &status, 0);
                }else { // if we cannot find all commands
                    fprintf(stderr, "The command %s or %s or %s doesn't exist\n",r.cmd[0][0], r.cmd[1][0], r.cmd[2][0]);
                }
            }
        }
    }
}
