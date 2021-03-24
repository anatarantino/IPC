//gcc -Wall -pedantic -std=c99 slave.c -o slave

#include <stdio.h>

#define MAX_SIZE 200

static void processFile(char * file);

int main(int argc, char const *argv[])
{
    //printf("Estoy en el slave\n");
    for(size_t i=1 ; i<argc ; i++){     //arranca en 1 por el nombre del programa en argv
        //printf("Estoy en el for del main del slave\n");
        processFile((char *)argv[i]);
    }
    return 0;
}

static void processFile(char * file){
    char buffer[MAX_SIZE];
    sprintf(buffer, "%s %s | grep -o -e \"Number of.*[0 - 9]\\+\" -e \"CPU time.*\" -e \".*SATISFIABLE\"",minisat,file);

    FILE *output;
    
    output=popen(buffer,"r");

    int dim=fread(buffer,sizeof(char),MAX_SIZE,output);

    buffer[dim]=0;

}
