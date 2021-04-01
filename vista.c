//gcc -Wall -pedantic -fsanitize=address -std=c99 vista.c -o vista
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_SIZE 4096
#define SHM_NAME "/shm_name"
#define SEM_NAME "/sem_name"
#define ERROR_HANDLER(message)  \
    do{                         \
        perror(message);        \
        exit(EXIT_FAILURE);     \
    }                           \
    while(0)                    \

int main(int argc, char const *argv[]){

    if(argc <= 1){
        ERROR_HANDLER("No files");
    }

    sem_t * sem = sem_open();


}
/*
static void initShm(){

} 

static void endShm(){

}

static void endSem(){

}
*/