// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#define _POSIX_C_SOURCE 2
#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_SIZE 4096
#define ERROR_HANDLER(message)  \
    do{                         \
        perror(message);        \
        exit(EXIT_FAILURE);     \
    }                           \
    while(0)                    \

static void processFile(char * file);

int main(int argc, char const *argv[])
{

    if (setvbuf(stdout, NULL, _IONBF, 0) != 0){
        ERROR_HANDLER("Error in function setvbuf - slave\n");
    }

    for(size_t i=1 ; i<argc ; i++){ 
        processFile((char *)argv[i]);
    }
    
    char file[MAX_SIZE + 1]={0};
    ssize_t count;
    while((count = read(STDIN_FILENO, file, MAX_SIZE)) != 0){ 
        if (count == -1){
            ERROR_HANDLER("Error in function read - slave\n");
        }

        file[count-1]=0; 
        processFile(file);
    }
    
    return 0;
}

static void processFile(char * file){
    char buffer[MAX_SIZE+1];
    
    if(sprintf(buffer, "minisat %s | grep -o -e \"Number of.*[0 - 9]\\+\" -e \"CPU time.*\" -e \".*SATISFIABLE\"",file)<0){
        ERROR_HANDLER("Error in function sprintf - slave\n");
    }

    FILE *output;
    
    if((output = popen(buffer,"r"))==NULL){
        ERROR_HANDLER("Error in function popen - slave\n");
    }

    size_t dim=fread(buffer,sizeof(char),MAX_SIZE,output);

    buffer[dim]=0;

    printf("File: %s\nPID: %d\n%s\n\n\t",file,getpid(),buffer); 
    
    if(pclose(output)==-1){
        ERROR_HANDLER("Error in function pclose - slave\n");
    }
}
