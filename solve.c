//gcc -Wall -pedantic -fsanitize=address -std=c99 solve.c -o solve

#define _SVID_SOURCE 1
#define _POSIX_C_SOURCE 200112L

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

#define MAX_SIZE 4096
#define ARGUM 6
#define READ 0
#define WRITE 1
#define STDIN 0
#define STDOUT 1
#define CANT_CHILD 8
#define FILES_PER_CHILD 5
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
    int pending_task;
}struct_slave;

static void initChildren(struct_slave *slaves, int files_per_child, int *total_files, char *argv[], int cant_child, int *file_count);
static void assignTask(int * pending_task, int fd_input,const char * files_array[], int * total_files);

int main(int argc, char const *argv[])
{
    int total_files=argc-1;

    if(total_files < 1) {
        fprintf(stderr,"No files\n");
        exit(EXIT_FAILURE);
    }
    //int cant_child=CANT_CHILD;
    //int files_per_child=5;

    char buffer[MAX_SIZE+1];
    int file_count=1;
    int read_count;

//    char * shmem = shmopen();
//    char * mmap = mmap();
    
    sleep(2); //esperar a que aparezca un proceso vista, si lo hace compartir argv

    struct_slave slaves[CANT_CHILD];
    initChildren(slaves, FILES_PER_CHILD, &total_files, (char**)(argv+1), CANT_CHILD, &file_count);
 
    fd_set read_fds;
    int max_fd_read=-1;
    while(total_files > 0){
        FD_ZERO(&read_fds);

        for(int i=0 ; i<CANT_CHILD ; i++) {
            FD_SET(slaves[i].output,&read_fds);
            if(slaves[i].output > max_fd_read){
                max_fd_read = slaves[i].output;
            }
        }

        int cant_fd = select(max_fd_read+1, &read_fds,NULL,NULL,NULL); //solo importan los fd de lectura
        if(cant_fd == -1){
            ERROR_HANDLER("Error in select function\n");
        }

        for(int i=0 ; i<CANT_CHILD ; i++) {
                
            if(FD_ISSET(slaves[i].output,&read_fds)){ 
                if((read_count=read(slaves[i].output,buffer,MAX_SIZE))==-1){
                    ERROR_HANDLER("Error in read function\n");   
                }
                if(read_count!=0){
                    buffer[read_count]=0;
                }
            
                for(char *j= buffer; (j=strchr(j,'\t'))!=NULL; j++){ 
                    slaves[i].pending_task--;  
                }
            
                //sendInfo();

                if(slaves[i].pending_task<=0){
                    assignTask(&(slaves[i].pending_task),slaves[i].input,argv+file_count,&total_files);     
                }
                //printf(buffer);

                //read leer lo que esta en el file descriptor e imprimir (ir al proceso vista)
                //le queda alguna tarea? entonces le mando una mas
                //si no le quedan mas tareas para procesar -> assignTask(&(slaves[i].pending_task),slaves[i].input,argv+file_count,&total_files);                
            }
        }
    }

        
    
    return 0;
}

static void initChildren(struct_slave slaves[CANT_CHILD], int files_per_child, int *total_files, char *argv[], int cant_child, int *file_count){ //VER!!!! por que hay que pasar slaves[CANT_CHILD] es necesario el CANT_CHILD??
    int childMaster[2],masterChild[2];
    pid_t pid;

    for(int i=0; i< cant_child; i++){
        slaves[i].pending_task=0;
        if(pipe(childMaster)==-1) {
            ERROR_HANDLER("Error creating pipe\n");
        }
        if(pipe(masterChild)==-1) {
            ERROR_HANDLER("Error creating pipe\n");
        }
        if((pid=fork())==0){   
            if(dup2(childMaster[WRITE],STDOUT)==-1){ //0 donde escribe el hijo eso quiero que vaya a la parte de escritura del pipe que es [1]
               // read en childMaster[0] va a leer hola (el padre)
               ERROR_HANDLER("Error dupping pipe\n");
            }
            if(close(childMaster[READ])==-1){
                ERROR_HANDLER("Error closing file descriptor");
            }
            if(dup2(masterChild[READ],STDIN)==-1){ //el hijo lee lo que le manda el padre
                ERROR_HANDLER("Error dupping pipe\n");
            }
            if(close(masterChild[WRITE])==-1){
                ERROR_HANDLER("Error closing file descriptor");
            }

            if(close(masterChild[READ])==-1){
                ERROR_HANDLER("Error closing file descriptor");
            }
            if(close(childMaster[WRITE])==-1){
                ERROR_HANDLER("Error closing file descriptor");
            }

            char * args[files_per_child + 2]; 
            int j=0;
            args[j++] = "slave";
            for(j=1 ; j<files_per_child + 1; j++){
              //  printf("----\nArchivo: %d-----\n",file_count);
                args[j] = argv[(*file_count)++];
            
            }
            args[j] = NULL;
        
            if(execv(args[0], args)==-1){
                ERROR_HANDLER("Error in execv function");
            }
        
        }else if(pid==-1){
            ERROR_HANDLER("Error creating a child\n");
        }

        slaves[i].input=masterChild[WRITE];
        slaves[i].output=childMaster[READ];
            //agregar las entradas que le enteresan al padre
            //osea los que no use aca
        slaves[i].pending_task+=files_per_child;
        slaves[i].pid=pid;
        *total_files-=files_per_child;
        *file_count+=files_per_child;

        if(close(masterChild[READ])==-1){
            ERROR_HANDLER("Error closing file descriptor");
        }
        if(close(childMaster[WRITE])==-1){
            ERROR_HANDLER("Error closing file descriptor");
        }

    }
} 

static void assignTask(int * pending_task, int fd_input,const char * files_array[], int * total_files){
    char buffer[MAX_SIZE];
    
    if(sprintf(buffer,"%s\n",files_array[0])<0){
        ERROR_HANDLER("Error in function sprintf\n");
    }
    
    if(write(fd_input,buffer,strlen(buffer))==-1){
        ERROR_HANDLER("Error in function write\n");
    }
    (*pending_task)++;
    (*total_files)--;
}

/*
static void sendInfo(){
    
}
*/

