// Name - Vaibhav Jindal
// Roll number - 170775
// Submission for CS330 Assignment 1 part 2

//necessary libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]){

    if(argc==4){// part 2.1
        // argv[1] must be '@'
        char *sy = "@";
        if(strcmp(argv[1],sy)!=0){
             printf("Wrong input\n");
             exit(1);
        }

        //pipe creation
        int fd[2];
        pipe(fd);

        pid_t pid;
        pid = fork();//fork
        
        if(pid==0){//child process
            dup2(fd[1],1);//dup STDOUT with fd[1],to send output of child to pipe
            
            //arg will be passed as input to execvp
            char *arg[5];
            arg[0] = "grep";
            arg[1] = "-rF";
            arg[2] = malloc(strlen(argv[2])+1);
            arg[3] = malloc(strlen(argv[3])+1);
            strcpy(arg[2],argv[2]);//key
            strcpy(arg[3],argv[3]);//path
            arg[4] = NULL;
            
            //exec grep
            execvp(arg[0], arg);
        }
        else{//parent process
            int wt = wait(NULL);//wait for child to complete
            dup2(fd[0],0);//dup STDIN with fd[0] to get input from pipe
            close(fd[1]);//close fd[1]
            char *arg[] = { "wc", "-l", NULL };//argument for exec in parent
            execvp(arg[0], arg); //execute exec
        }
    }

    else{
        //argv[1] must be dollar
        char *sy = "$";
        if(strcmp(argv[1],sy)!=0){
            printf("Wrong input\n");
            exit(1);
        }

        //pipe creation
        int fd[2];
        pipe(fd);
        pid_t pid = fork();//1st fork
        
        if(pid==0){//child 1
            dup2(fd[1],1);//dup STDOUT with fd[1],to send output of child to pipe
            
            //arg will be passed as input to execvp
            char *arg[5];
            arg[0] = "grep";
            arg[1] = "-rF";
            arg[2] = malloc(strlen(argv[2])+1);
            arg[3] = malloc(strlen(argv[3])+1);
            strcpy(arg[2],argv[2]);//key
            strcpy(arg[3],argv[3]);//path
            arg[4] = NULL;

            //exec grep
            execvp(arg[0],arg);
        }
        else{// parent process
            int wt = wait(NULL);//wait for child 1 to complete
            pid_t pid2 = fork();// create child 2
            if(pid2==0){// child 2 sends output of grep to file
                char *arg[5];//arg contains arguments for exec
                arg[0] = "grep";
                arg[1] = "-rF";
                arg[2] = malloc(strlen(argv[2])+1);
                arg[3] = malloc(strlen(argv[3])+1);
                strcpy(arg[2],argv[2]);//key
                strcpy(arg[3],argv[3]);//path
                arg[4] = NULL;
                int file = open(argv[4], O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);//open file with appropriate flags
                dup2(file,1);//send output to file, not to STDOUT
                close(2);//close stderr
                execvp(arg[0],arg);//exec
            }
            else{
                int wt2 = wait(NULL);//wait for child 2 to complete
                dup2(fd[0],0);//dup STDIN with fd[0] to get input from pipe
                close(fd[1]);//close fd[1]
                
                //creating arguments for exec
                int n = argc - 4;
                char *arg[n];
                for(int i=0;i<n-1;i++){//to get all the flags from the command
                    arg[i] = malloc(strlen(argv[5+i])+1);
                    strcpy(arg[i],argv[5+i]);
                }
                arg[n-1] = NULL;
                execvp(arg[0], arg);//exec 
            }
        }
    }

    return 0;
}