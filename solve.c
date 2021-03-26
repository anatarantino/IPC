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


#define MAX_INPUT_SIZE 400
#define ARGUM 6
#define CANT_SONS 8

typedef struct{
    pid_t pid;
    int input;
    int output;
    int tasks_done;
}struct_slave;

static void initSlaves(struct_slave *slaves, int files_per_son, int *total_files, char *argv[], int *cant_sons, int *file_count);
static int assignTask(int fd_input,char * files_array[],int cant_files);
static int min(int a, int b);

int main(int argc, char const *argv[])
{
    int total_files=argc-1;
    if(total_files < 1) {
        fprintf(stderr,"no hay archivos\n");
        exit(-1);
    }
    int cant_sons=CANT_SONS;
    int files_per_son=5;
    //int cant_sons=abs(total_files*0.05) + 1; //sumo 1 por si la cant de archivos es menor a 20 // SON 4 !!
    //int files_per_son=abs(total_files*0.02) + 1; //sumo 1 por si la cant de archivos es menor a 50 // SON 2!!
    //char* buffer[total_files][ARGUM];
    int file_count=1;
    int index=0;

//    char * shmem = shmopen();
//    char * mmap = mmap();
    
    sleep(2); //esperar a que aparezca un proceso vista, si lo hace compartir argv

    struct_slave slaves[CANT_SONS];
    initSlaves(slaves, files_per_son, &total_files, (char**)(argv+1), &cant_sons, &file_count);

    while(total_files > 0){
        for(int i=0 ; i<cant_sons ; i++) {
            
            slaves[i].tasks_done += (slaves[i].input,argv+file_count,min(files_per_son,total_files-file_count));

        }
    }

    return 0;
}

static void initSlaves(struct_slave slaves[CANT_SONS], int files_per_son, int *total_files, char *argv[], int *cant_sons, int *file_count){
    int sonMaster[2],masterSon[2];
    pid_t pid;

    for(int i=0; i< *cant_sons; i++){
        slaves[i].tasks_done=0;
        if(pipe(sonMaster)< 0) {
            printf("Error creating pipe\n");
            exit(1);
        }
        if(pipe(masterSon)< 0) {
            printf("Error creating pipe\n");
            exit(1);
        }
        if((pid=fork())==0){   
            if(dup2(sonMaster[1],0)<0){ //0 donde escribe el hijo eso quiero que vaya a la parte de escritura del pipe que es [1]
               // read en sonMaster[0] va a leer hola (el padre)
               printf("Error dupping pipe\n");
                exit(1);
            }
            close(sonMaster[0]);
            if(dup2(masterSon[0],1)<0){ //el hijo lee lo que le manda el padre
                printf("Error dupping pipe\n");
                exit(1);
            }
            close(masterSon[1]);
            char * args[files_per_son + 2]; 
            int j=0;
            args[j++] = "slave";
            while(j<files_per_son+1){
              //  printf("----\nArchivo: %d-----\n",file_count);
                //printf("execv\n");
                args[j] = argv[(*file_count)++];
                j++;
            }
            args[j] = NULL;
            if(execv(args[0], args) < 0){
                exit(-1);
            }
        }

        slaves[i].input=masterSon[1];
        slaves[i].output=sonMaster[0];
            //agregar las entradas que le enteresan al padre
            //osea los que no use aca
        slaves[i].tasks_done+=files_per_son;
        slaves[i].pid=pid;
        *total_files-=files_per_son;
        *file_count+=files_per_son;

        close(masterSon[0]);
        close(sonMaster[1]);

    }
} 

static int assignTask(int fd_input,char * files_array[],int cant_files){

    char buffer[MAX_INPUT_SIZE];
    for(int i=0 ; i<cant_files ; i++){
        sprintf(buffer,"%s\n",files_array[i]);
    }
    write(fd_input,buffer,strlen(buffer));
    return cant_files;
}

static int min(int a, int b){
    if(a<=b){
        return a;
    }
    return b;
}