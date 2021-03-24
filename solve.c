//gcc -Wall -pedantic -fsanitize=address -std=c99 solve.c -o solve

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#define ARGUM 6

int main(int argc, char const *argv[])
{
    int total_files=argc-1;
    if(total_files < 1) {
        fprintf(stderr,"no hay archivos\n");
        exit(-1);
    }
    int cant_sons=8;
    int files_per_son=5;
    //int cant_sons=abs(total_files*0.05) + 1; //sumo 1 por si la cant de archivos es menor a 20 // SON 4 !!
    //int files_per_son=abs(total_files*0.02) + 1; //sumo 1 por si la cant de archivos es menor a 50 // SON 2!!
    int random;
    char* buffer[total_files][ARGUM];
    int file_count=1;
    int index=0;

    char * shmem = shmopen();
    char * mmap = mmap();
    
    sleep(2); //esperar a que aparezca un proceso vista, si lo hace compartir argv
    




    for(int i=0; i< cant_sons && total_files>0; i++){
        //printf("entre al for\n");

        if(fork()==0){
            //printf("Hola soy un hijo\n");
            char * args[files_per_son + 2]; 
            int j=0;
            args[j++] = "slave";
            while(j<files_per_son+1){
                printf("----\nArchivo: %d-----\n",file_count);
                //printf("execv\n");
                args[j] = argv[file_count++];
                j++;
            }
            args[j] = NULL;
            if(execv(args[0], args) < 0){
                    exit(-1);
            }
            total_files-=files_per_son;
        }
    }

    int childPID, exitStatus;
    while ((childPID = wait(&exitStatus)) > 0 && total_files>0) {//son finishes
        buffer[index][0]=index; //reemplazar index por valor de retorno de hijo
        index++;
        total_files--;
    }

    return 0;
}
