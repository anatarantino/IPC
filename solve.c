#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    if(argc <= 1) {
        fprintf(stderr,"no hay archivos\n");
        exit(-1);
    }
    int total_files=argc-1;
    
    int cant_sons=(int)total_files*0.05 + 1; //sumo 1 por si la cant de archivos es menor a 20
    int cant_files=(int)total_files*0.02 + 1; //sumo 1 por si la cant de archivos es menor a 50
    int random;
    int buffer[total_files];
    int index=0;
    
    sleep(2); //esperar a que aparezca un proceso vista, si lo hace compartir argv
    
    for(int i=0; i< cant_sons; i++){
        printf("entre al for\n");
        random=rand()%5;
        if(fork()==0){
            printf("Hola soy un hijo\n");
            total_files-=cant_files;
            sleep(random);
        }
    }

    int childPID, exitStatus;
    while ((childPID = wait(&exitStatus)) > 0 && total_files>0) {//son finish
        buffer[index]=index; //reemplazar index por valor de retorno de hijo
        index++;
        printf("Estoy terminando, voy a agarrar un nuevo archivo\n");
        total_files--;
    }

    return 0;
}
