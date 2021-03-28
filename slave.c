//gcc -Wall -pedantic -fsanitize=address -std=c99 slave.c -o slave

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
    //printf("Estoy en el slave\n");

    if (setvbuf(stdout, NULL, _IONBF, 0) != 0){
        ERROR_HANDLER("slave");
    }

    for(size_t i=1 ; i<argc ; i++){     //arranca en 1 por el nombre del programa en argv
        //printf("Estoy en el for del main del slave\n");
        processFile((char *)argv[i]);
    }
    
    return 0;
}

static void processFile(char * file){
    char buffer[MAX_SIZE];
    if(sprintf(buffer, "%s %s | grep -o -e \"Number of.*[0 - 9]\\+\" -e \"CPU time.*\" -e \".*SATISFIABLE\"","minisat",file)<0){
        ERROR_HANDLER("Error in function sprintf\n");
    }

    FILE *output;
    
    if((output = popen(buffer,"r"))==NULL){
        ERROR_HANDLER("Error in function popen\n");
    }

    int dim=fread(buffer,sizeof(char),MAX_SIZE,output);

    buffer[dim]=0;

    printf("File: %s\nPID: %d\n%s\n\n\t",file,getpid(),buffer); //para no usar el \n
    //dprintf(2,"File: %s\nPID: %d\n%s\n\n%c",file,getpid(),buffer,7); //para no usar el \n
    if(pclose(output)==-1){
        ERROR_HANDLER("Error in function pclose\n");
    }

}
