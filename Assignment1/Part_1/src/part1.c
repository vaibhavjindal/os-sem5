// Name - Vaibhav Jindal
// Roll number - 170775
// Submission for CS330 Assignment 1 part 1

//necessary libraries
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

// function to concatenate two strings and return a pointer to the new concatenated string
char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

// function that searches 'key' in the file 'fpath'
// flag = 0 when file name has to be printed in front of search results
void mygrep(char *fpath, char *key,int flag){
    
    struct stat *statbuf;//to extract file size
    statbuf = malloc(sizeof(struct stat));
    int t = stat(fpath,statbuf);
    long long int fsize = statbuf->st_size;//fsize contains the file size
    
    //to store file contents, +1 for null terminator
    char *content  = (char *) malloc((fsize+1)*sizeof(char));
    
    //opening file
    int fd = open(fpath,O_RDONLY); 
    if (fd < 0)  
    {   
        perror(fpath); 
        exit(1); 
    }

    //reading file and storing the data in content  
    int sz = read(fd,content,fsize);
    content[fsize]='\0';

    //breaking the file at every newline and searching for key in every line
    char *token = strtok(content, "\n"); 
    while (token != NULL) 
    { 
        if(strstr(token,key)!=NULL){//strstr returns a pointer to the first occurence of key in token
            if(flag==0) printf("%s:%s\n",fpath,token);
            else printf("%s\n",token);//when file name should not be printed
        }
        token = strtok(NULL, "\n"); 
    } 

    //closing the file
    free(content); 
    if (close(fd) < 0)  
    { 
        perror(fpath); 
        exit(1); 
    }  
}

//recursive function to traverse the directory structure
void func(char* path,char* key){
    DIR *dir;
    struct dirent *pDirent;

    dir = opendir(path);
    if (dir == NULL) {// when path points to a file, call mygrep function
        mygrep(path,key,0);
        return;        
    }

    while ((pDirent = readdir(dir)) != NULL) {// when path points to a directory, read its contents
        char *s = pDirent->d_name;
        
        char *temp = ".";
        char *temp2 = "..";
        
        char *temp3 = "/";
        if(strcmp(s,temp)!=0 && strcmp(s,temp2)!=0){//no need to recursively view '.' and '..'
            func(concat(concat(path,temp3),s),key);// recursion for next sub directory
        }
    }
    closedir (dir);

}


int main(int argc, char *argv[])
{
    if (argc != 3) {// three arguments must be there
    fprintf(stderr, "wrong input\n");
	exit(1);
    }

    char *key = argv[1];
    char *path = argv[2];
    
    //if the path ends with a '/', remove the '/'
    int l = strlen(path);
    if(path[l-1]=='/') path[l-1]='\0';
    
    //the variable f tracks if the given path is a file
    int f=0;
    DIR *dir;
    dir = opendir(path);
    if (dir == NULL) {
        mygrep(path,key,1);
        f=1;
    }
    closedir(dir);

    //when the given path points to a directory
    if(f==0) func(path,key);

    return 0;
}