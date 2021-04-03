//gcc -Wall -pedantic -fsanitize=address -std=c99 vista.c -o vista
#include <fcntl.h> 
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_LEN 5   
#define MAX_SIZE 4096
#define SHM_NAME "/shm_name"
#define SEM_NAME "/sem_name"
#define PERM 0666
#define ERROR_HANDLER(message)  \
    do{                         \
        perror(message);        \
        exit(EXIT_FAILURE);     \
    }                           \
    while(0)                    \

static char * initShm(const char *shm_name, int shm_oflag, mode_t mode, size_t size, int *shm_fd);
static void closure(void * smap, int shm_fd, size_t size,char * shm_name, sem_t * sem, char * sem_name);

int main(int argc, char const *argv[]){

    int total_files;

    if(argc <= 1){
        char stdin_buffer[MAX_LEN];
        size_t count;
        if((count=read(STDIN_FILENO, stdin_buffer, MAX_LEN)) == -1){
            ERROR_HANDLER("Error in function read");
        }
        stdin_buffer[count]=0;
        total_files = atoi(stdin_buffer);
    }
    else{
        total_files = atoi(argv[1]);
    }
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, 0);

    //sem_t * sem = sem_open(SEM_NAME, O_CREAT, PERM, 0);
    if(sem == SEM_FAILED){
        ERROR_HANDLER("Error in function sem_open");
    }
    int shm_fd;
    void * smap = initShm(SHM_NAME, O_CREAT | O_RDWR, PERM,total_files * MAX_SIZE, &shm_fd);
    
    char *map_pointer = smap;

    int counter = 0;
    while(counter < total_files){
        int i = -1;
        sem_getvalue(sem,&i);
        printf("calor: %d\n",i);
        if(sem_wait(sem) == -1){
            ERROR_HANDLER("Error in function sem_wait");
        }

        char * next;

        if((next=strchr(map_pointer,'\t'))==NULL){
            ERROR_HANDLER("Error in function strchr");
        }
        *next='\0';
        next++;
        printf("%s\n", map_pointer);
        map_pointer = next;
    }

    closure(smap, shm_fd, MAX_SIZE, SHM_NAME, sem, SEM_NAME);
}

static char * initShm(const char *shm_name, int shm_oflag, mode_t mode, size_t size, int *shm_fd){
    *shm_fd = shm_open(shm_name, shm_oflag, mode); 
    if((*shm_fd) == -1){
        ERROR_HANDLER("Error in function shm_open - vista");
    }

    char * smap = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, *shm_fd, 0);
    if(smap == MAP_FAILED){
        ERROR_HANDLER("Error in function mmap - vista");
    }

    return smap;

} 

static void closure(void * smap, int shm_fd, size_t size,char * shm_name, sem_t * sem, char * sem_name){
    if(sem_close(sem) == -1){
        ERROR_HANDLER("Error in function sem_close");
    }
    if(sem_unlink(sem_name) == -1){
        ERROR_HANDLER("Error in function sem_unlink");
    }
    if(munmap(smap,size) == -1){
        ERROR_HANDLER("Error in function munmap - vista");
    }

    if(close(shm_fd) == -1){
        ERROR_HANDLER("Error in function close");
    }

    if(shm_unlink(shm_name) == -1){
        ERROR_HANDLER("Error in function shm_unlink");
    }

} 