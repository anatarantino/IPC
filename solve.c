// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#define _SVID_SOURCE 1
#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>  
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
#define SHM_NAME "/shm_name"
#define SEM_NAME "/sem_name"
#define PERM 0666
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

static void initChildren(struct_slave *children, int files_per_child, char *argv[], int cant_child, int *index);
static void assignTask(int * pending_task, int fd_input,const char * files_array[], int * file_count);
static void * initShm(const char *name, int oflag, mode_t mode, size_t size, int *shm_fd);
static void closure(void * smap, int shm_fd, size_t size,char * shm_name, sem_t * sem, char * sem_name, struct_slave children[], size_t cant_children, FILE * output_file);
static void closeChildren(struct_slave children[], size_t cant_children);
static void sendInfo(char * buffer, FILE * output_file, char * smap, size_t * smap_count, sem_t * sem_name, int cant_task);

int main(int argc, char const *argv[])
{
    int total_files=argc-1;

    if(total_files < 1) {
        ERROR_HANDLER("No files - solve\n");
    }

    int processed_tasks=0;
    char buffer[MAX_SIZE+1];
    int read_count;
    int index_for_tasks = 0;
    int shm_fd;
    int map_size = total_files * MAX_SIZE;
    
    void * smap = initShm(SHM_NAME,O_CREAT | O_RDWR, PERM,map_size, &shm_fd); 
    sem_t * sem = sem_open(SEM_NAME, O_CREAT, PERM, 0);
    
    if(sem == SEM_FAILED){
        ERROR_HANDLER("Error in function sem_open - solve\n");
    }
    
    FILE * output_file = fopen("output.txt","w");
    if(output_file == NULL){
        ERROR_HANDLER("Error in function fopen - solve\n");
    }

    int init_files_count=FILES_PER_CHILD, init_children_count=CANT_CHILD;
    if(CANT_CHILD*FILES_PER_CHILD>total_files){
        init_children_count = 1;
        init_files_count = 1;
    }

    printf("%d", total_files);
    sleep(2);
    struct_slave children[init_children_count];

    initChildren(children, init_files_count, (char**)(argv+1), init_children_count, &index_for_tasks);

    fd_set read_fds;
    int max_fd_read=-1;
    size_t smap_count=0;

    while(processed_tasks < total_files){ 

        FD_ZERO(&read_fds);

        for(int i=0 ; i<init_children_count ; i++) {
            FD_SET(children[i].output,&read_fds);
            if(children[i].output > max_fd_read){
                max_fd_read = children[i].output;
            }
        }

        int cant_fd = select(max_fd_read+1, &read_fds,NULL,NULL,NULL);
        if(cant_fd == -1){
            ERROR_HANDLER("Error in select function - solve\n");
        }

        for(int i=0 ; i<init_children_count; i++) {
                
            if(FD_ISSET(children[i].output,&read_fds)){ 
                if((read_count=read(children[i].output,buffer,MAX_SIZE))==-1){
                    ERROR_HANDLER("Error in read function - solve\n");   
                }
                if(read_count!=0){
                    buffer[read_count]=0;
                
                    int cant_task=0;

                    for(char *j= buffer; (j=strchr(j,'\t'))!=NULL; j++){ 
                        children[i].pending_task--;
                        cant_task++;
                        processed_tasks++;
                    }

                    sendInfo(buffer, output_file,smap,&smap_count,sem,cant_task);

                    if(children[i].pending_task<=0){
                        assignTask(&(children[i].pending_task),children[i].input,argv+processed_tasks,&index_for_tasks);
                        index_for_tasks++;
                    }
                }              
            }
        }      
    }
    closure(smap, shm_fd, map_size,SHM_NAME, sem, SEM_NAME, children,init_children_count,output_file); 
    return 0;
}

static void initChildren(struct_slave children[], int files_per_child, char *argv[], int cant_child, int *index){ 
    int childMaster[2],masterChild[2];
    pid_t pid;

    for(int i=0; i< cant_child; i++){
        children[i].pending_task=0;
        if(pipe(childMaster)==-1) {
            ERROR_HANDLER("Error creating pipe - solve\n");
        }
        if(pipe(masterChild)==-1) {
            ERROR_HANDLER("Error creating pipe - solve\n");
        }
        if((pid=fork())==0){   
            if(dup2(childMaster[WRITE],STDOUT)==-1){ 
               ERROR_HANDLER("Error dupping pipe - solve\n");
            }
            if(close(childMaster[READ])==-1){
                ERROR_HANDLER("Error closing file descriptor - solve\n");
            }
            if(dup2(masterChild[READ],STDIN)==-1){ 
                ERROR_HANDLER("Error dupping pipe - solve\n");
            }
            if(close(masterChild[WRITE])==-1){
                ERROR_HANDLER("Error closing file descriptor - solve\n");
            }

            if(close(masterChild[READ])==-1){
                ERROR_HANDLER("Error closing file descriptor - solve\n");
            }
            if(close(childMaster[WRITE])==-1){
                ERROR_HANDLER("Error closing file descriptor - solve\n");
            }

            char * args[files_per_child + 2]; 
            int j=0;
            args[j++] = "slave.out";
            for(int k=0; k<files_per_child; j++,k++){ 
                args[j] = argv[(*index)++];
            }
            args[j] = NULL;
            
            if(execv(args[0], args)==-1){
                ERROR_HANDLER("Error in execv function - solve\n");
            }
        
        }else if(pid==-1){
            ERROR_HANDLER("Error creating a child - solve\n");
        }

        children[i].input=masterChild[WRITE];
        children[i].output=childMaster[READ];

        children[i].pending_task+=files_per_child;
        children[i].pid=pid;
        (*index)+=files_per_child;

        if(close(masterChild[READ])==-1){
            ERROR_HANDLER("Error closing file descriptor - solve\n");
        }
        if(close(childMaster[WRITE])==-1){
            ERROR_HANDLER("Error closing file descriptor - solve\n");
        }

    }
} 

static void assignTask(int * pending_task, int fd_input,const char * files_array[], int * file_count){
    char buffer[MAX_SIZE];
    
    if(sprintf(buffer,"%s\n",files_array[0])<0){
        ERROR_HANDLER("Error in function sprintf - solve\n");
    }
    
    if(write(fd_input,buffer,strlen(buffer))==-1){
        ERROR_HANDLER("Error in function write - solve\n");
    }
    (*pending_task)++;
    (*file_count)++;
}

static void * initShm(const char *name, int oflag, mode_t mode, size_t size, int *shm_fd){ 
    *shm_fd = shm_open(name, oflag, mode);
    if((*shm_fd) == -1){
        ERROR_HANDLER("Error in function shm_open - solve\n");
    }
    if(ftruncate(*shm_fd, size) == -1)
        ERROR_HANDLER("Error in function ftruncate - solve\n");
        
    void * smap = mmap(NULL, size, PROT_WRITE, MAP_SHARED, *shm_fd, 0);
    if(smap == MAP_FAILED){
        ERROR_HANDLER("Error in function mmap - solve\n");
    }

    return smap;
}

static void sendInfo(char * buffer, FILE * output_file, char * smap, size_t * smap_count, sem_t * sem, int cant_task){
    size_t size = strlen(buffer);

    if(fwrite(buffer, size, sizeof(char), output_file) == 0){
        ERROR_HANDLER("Error in function fwrite - solve\n");
    }

    memcpy(smap + *smap_count,buffer,size);
    (*smap_count)+=size;

    for (size_t i = 0; i<cant_task ; i++){
        if(sem_post(sem) == -1){
            ERROR_HANDLER("Error in function sem_post - solve\n");
        }
        int s = -1;
        sem_getvalue(sem, &s);
    }   

}

static void closure(void * smap, int shm_fd, size_t size,char * shm_name, sem_t * sem, char * sem_name, struct_slave children[], size_t cant_children, FILE * output_file){
    if(munmap(smap,size) == -1){
        ERROR_HANDLER("Error in function munmap - solve\n");
    }

    if(close(shm_fd) == -1){
        ERROR_HANDLER("Error in function close - solve\n");
    }

    closeChildren(children, cant_children);

    if(fclose(output_file) == EOF){
        ERROR_HANDLER("Error in function fclose - solve\n");
    }

    if(sem_close(sem) == -1){
        ERROR_HANDLER("Error in function sem_close - solve\n");
    }
} 

static void closeChildren(struct_slave children[], size_t cant_children){
    for(size_t i=0 ; i<cant_children ; i++){
        if(close(children[i].input) == -1){
            ERROR_HANDLER("Error in function close - children input - solve\n");
        }
        if(close(children[i].output) == -1){
            ERROR_HANDLER("Error in function close - children output - solve\n");
        }
    }
}
