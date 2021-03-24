#include <stdio.h>

static void processFile(char * file);

int main(int argc, char const *argv[])
{
    for(size_t i=1 ; i<argc ; i++){ //arranca en 1 por el nombre del programa en argv
        processFile((char *)argv[i]);
    }
    return 0;
}

static void processFile(char * file){

}
