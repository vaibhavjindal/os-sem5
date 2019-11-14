// Name - Vaibhav Jindal
// Roll number - 170775
// Submission for CS330 Assignment 1 part 3

//necessary libraries
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

// function to concatenate two strings and return a pointer to the new concatenated string
char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);// +1 for the null-terminator
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

//function to return the file size of a file
long long int mysum(char *fpath){
    struct stat *statbuf;//*statbuf will contain the stat info of the file 
    statbuf = malloc(sizeof(struct stat));
    int t = stat(fpath,statbuf);
    long long int fsize = statbuf->st_size;//fsize is the required file size
    free(statbuf);
    return fsize;
}

//function to recursively visits the directory tree and adds up the file sizes
long long int func(char* path){
    DIR *dir;
    struct dirent *pDirent;

    dir = opendir(path);
    if (dir == NULL) {// when path points to a file, invoke mysum
        return mysum(path);  
    }

    long long int sum=0;// sum stores the sum of file sizes of the files in the directory
    while ((pDirent = readdir(dir)) != NULL) {
        char *s = pDirent->d_name;
        char *temp = ".";
        char *temp2 = "..";
        char *temp3 = "/";
        if(strcmp(s,temp)!=0 && strcmp(s,temp2)!=0){//no need to recursively view '.' and '..'
            sum += func(concat(concat(path,temp3),s));// recursion for next sub directory
        }
    }
    closedir (dir);
    return sum;
}


int main(int argc, char *argv[])
{
    if (argc != 2) {// number of arguments must be two
    fprintf(stderr, "wrong input\n");
	exit(1);
    }

    char *path = argv[1];
    
    //if the path ends with a '/', remove the '/'
    int l = strlen(path);
    if(path[l-1]=='/') path[l-1]='\0';
    
    DIR *dir;
    dir = opendir(path);

    //not a valid directory
    if (dir == NULL) {
        perror("Not a valid directory.");
        exit(1);
    }
    closedir(dir);

    //when directory path is valid
    dir = opendir(path);
    struct dirent *pDirent;
    
    //to extract only the directory name in the file path
    int ind = -1;
    for(int i=l-2;i>-1;i--){
        if(path[i]=='/'){
            ind = i;
            break;
        }
    }
    char *s = malloc(l);
    if(ind==-1) s = path;
    else s = path+ind+1;
    
    //print the size of the parent directory
    printf("%s %lld\n",s,func(path));

    //for the sub directories
    while ((pDirent = readdir(dir)) != NULL) {
        char *s = pDirent->d_name;
        char *temp = ".";
        char *temp2 = "..";
        char *temp3 = "/";
        if(strcmp(s,temp)!=0 && strcmp(s,temp2)!=0){//no need to recursively view '.' and '..'
            char *temp_path = concat(concat(path,temp3),s);//to get the path of subdirectory or file
            DIR *temp_dir = opendir(temp_path);
            if(temp_dir!=NULL){//only when subdirectory, we need to print result
                int pid = fork();// fork for each subdirectory
                if(pid==0){
                    printf("%s %lld\n",s,func(temp_path));// print for each child process
                    break;// break the loop in case of a child process
                }
		else{//wait for child process to complete
			int wc = wait(NULL);
		}
            }
        }
    }
    

    return 0;
}
