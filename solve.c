//gcc -Wall -pedantic -fsanitize=address -std=c99 solve.c -o solve

#define _SVID_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>

#define _POSIX_C_SOURCE 200112L
#define MAX_SIZE 40
#define ARGUM 6
#define CANT_CHILD 8
#define ERROR_HANDLER(message)  \
    do{                         \
        perror(message);        \
        exit(EXIT_FAILURE);     \
    }                           \
    while(0)                    \

typedef struct{
    pid_t pid;
    int input;
    int output;
    int pending_done;
}struct_slave;

static void initChildren(struct_slave *slaves, int files_per_child, int *total_files, char *argv[], int *cant_child, int *file_count);
static void assignTask(int * pending_done, int fd_input,char * files_array[], int * total_files);

int main(int argc, char const *argv[])
{
    int total_files=argc-1;
    if(total_files < 1) {
        fprintf(stderr,"no hay archivos\n");
        exit(-1);
    }
    int cant_child=CANT_CHILD;
    int files_per_child=5;
    //int cant_child=abs(total_files*0.05) + 1; //sumo 1 por si la cant de archivos es menor a 20 // child 4 !!
    //int files_per_child=abs(total_files*0.02) + 1; //sumo 1 por si la cant de archivos es menor a 50 // child 2!!
    char buffer[MAX_SIZE];
    int file_count=1;
    int index=0;
    int read_count;

//    char * shmem = shmopen();
//    char * mmap = mmap();
    
    sleep(2); //esperar a que aparezca un proceso vista, si lo hace compartir argv

    struct_slave slaves[CANT_CHILD];
    initChildren(slaves, files_per_child, &total_files, (char**)(argv+1), &cant_child, &file_count);
    fd_set read_fds;
    int max_fd_read=-1;
    while(total_files > 0){
        FD_ZERO(&read_fds);

        for(int i=0 ; i<cant_child ; i++) {
            FD_SET(slaves[i].output,&read_fds);
            if(slaves[i].output > max_fd_read){
                max_fd_read = slaves[i].output;
            }
        }

        int cant_fd = select(max_fd_read+1, &read_fds,NULL,NULL,NULL); //solo importan los fd de lectura
        if(cant_fd == -1){
            ERROR_HANDLER("");
        }

        for(int i=0 ; i<cant_child ; i++) {

            int fd_read = slaves[i].output;
            if(FD_ISSET(fd_read,&read_fds)){
                if((read_count=read(fd_read,buffer,MAX_SIZE))==-1){
                    ERROR_HANDLER("");   
                }
                }
                buffer[read_count]=0;
                buffer[read_count]=0;
                printf("%s",buffer);


           //     sendInfo();
           //     if()
                //read leer lo que esta en el file descriptor e imprimir (ir al proceso vista)
                //le queda alguna tarea? entonces le mando una mas
                //si no le quedan mas tareas para procesar -> assignTask(&(slaves[i].pending_done),slaves[i].input,argv+file_count,&total_files);                
            }
        }
        
    

    return 0;
}

static void initChildren(struct_slave slaves[CANT_CHILD], int files_per_child, int *total_files, char *argv[], int *cant_child, int *file_count){
    int childMaster[2],masterChild[2];
    pid_t pid;

    for(int i=0; i< *cant_child; i++){
        slaves[i].pending_done=0;
        if(pipe(childMaster)< 0) {
            printf("Error creating pipe\n");
            exit(1);
        }
        if(pipe(masterChild)< 0) {
            printf("Error creating pipe\n");
            exit(1);
        }
        if((pid=fork())==0){   
            if(dup2(childMaster[1],0)<0){ //0 donde escribe el hijo eso quiero que vaya a la parte de escritura del pipe que es [1]
               // read en childMaster[0] va a leer hola (el padre)
               printf("Error dupping pipe\n");
                exit(1);
            }
            close(childMaster[0]);
            if(dup2(masterChild[0],1)<0){ //el hijo lee lo que le manda el padre
                printf("Error dupping pipe\n");
                exit(1);
            }
            close(masterChild[1]);
            char * args[files_per_child + 2]; 
            int j=0;
            args[j++] = "slave";
            while(j<files_per_child+1){
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

        slaves[i].input=masterChild[1];
        slaves[i].output=childMaster[0];
            //agregar las entradas que le enteresan al padre
            //osea los que no use aca
        slaves[i].pending_done+=files_per_child;
        slaves[i].pid=pid;
        *total_files-=files_per_child;
        *file_count+=files_per_child;

        close(masterChild[0]);
        close(childMaster[1]);

    }
} 

static void assignTask(int * pending_done, int fd_input,char * files_array[], int * total_files){

    char buffer[MAX_SIZE];
    
    sprintf(buffer,"%s\n",files_array[0]);
    
    write(fd_input,buffer,strlen(buffer));
    (*pending_done)++;
    (*total_files)--;
}

static void sendInfo(){
    
}