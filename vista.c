//gcc -Wall -pedantic -fsanitize=address -std=c99 vista.c -o vista
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

int main(int argc, char const *argv[]){



}
/*
static void initShm(){

}

static void endShm(){

}

static void endSem(){

}
*/