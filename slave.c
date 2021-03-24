//gcc -Wall -pedantic -std=c99 slave.c -o slave

#include <stdio.h>

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
    //printf("Entre al slaveeee / Process file\n");
    minisat(*file);
}
