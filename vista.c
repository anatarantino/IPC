// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

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
        size_t count = 0;
        if((count=read(STDIN_FILENO, stdin_buffer, MAX_LEN)) == -1){
            ERROR_HANDLER("Error in function read - vista\n");
        }
        stdin_buffer[count]=0;
        total_files = atoi(stdin_buffer);
    }
    else{
        total_files = atoi(argv[1]);
    }

    if(total_files <= 0){
        ERROR_HANDLER("Error in receiving total_files - vista\n"); 
    }

    sem_t *sem = sem_open(SEM_NAME, O_CREAT, PERM, 0);

    if(sem == SEM_FAILED){
        ERROR_HANDLER("Error in function sem_open - vista\n");
    }
    int shm_fd;
    
    char * smap = initShm(SHM_NAME, O_CREAT | O_RDWR, PERM,total_files * MAX_SIZE, &shm_fd);

    char *map_pointer = smap;

    int counter = 0;
    
    while(counter < total_files){
        int s = -1;
        if(sem_wait(sem) == -1){
            ERROR_HANDLER("Error in function sem_wait - vista\n");
        }
        char * next;
        sem_getvalue(sem, &s);
        if((next=strchr(map_pointer,'\t'))==NULL){
            ERROR_HANDLER("Error in function strchr - vista\n");
        }
        *next='\0';
        printf("%s",map_pointer);
        next++;
        counter++;
        map_pointer = next; 
    }
    closure(smap, shm_fd, total_files * MAX_SIZE, SHM_NAME, sem, SEM_NAME);
    return 0;
}

static char * initShm(const char *shm_name, int shm_oflag, mode_t mode, size_t size, int *shm_fd){
    *shm_fd = shm_open(shm_name, shm_oflag, mode); 
    if((*shm_fd) == -1){
        ERROR_HANDLER("Error in function shm_open - vista\n");
    }

    char * smap = mmap(NULL, size, PROT_WRITE, MAP_SHARED, *shm_fd, 0);
    if(smap == MAP_FAILED){
        ERROR_HANDLER("Error in function mmap - vista\n");
    }
    return smap;
} 

static void closure(void * smap, int shm_fd, size_t size,char * shm_name, sem_t * sem, char * sem_name){
    if(sem_close(sem) == -1){
        ERROR_HANDLER("Error in function sem_close - vista\n");
    }
    if(sem_unlink(sem_name) == -1){
        ERROR_HANDLER("Error in function sem_unlink - vista\n");
    }
    if(munmap(smap,size) == -1){
        ERROR_HANDLER("Error in function munmap - vista\n");
    }
    if(close(shm_fd) == -1){
        ERROR_HANDLER("Error in function close - vista\n");
    }
    if(shm_unlink(shm_name) == -1){
        ERROR_HANDLER("Error in function shm_unlink - vista\n");
    }
} 