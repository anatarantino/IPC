TP1: Inter Process Communication - SO 2021

Equipo:
    - Prado Torres, Macarena
    - Rodriguez, Martina
    - Tarantino Pedrosa, Ana

Objetivo:
    El objetivo de este trabajo es distribuir tareas de SAT solving entre varios procesos utilizando los distintos tipos de IPCs presentes en un sistema POSIX.

Instalaciones necesarias:

    Es necesario tener instalado minisat para poder ejecutar el programa. Para instalarlo puede utilizar lo siguiente:
        apt-get install minisat

    Luego, para realizar los testeos es necesario tener instalado pvs-studio, cppcheck y valgrind. Para instalarlos:
        apt-get install pvs-studio
        apt-get install cppcheck
        apt-get install valgrind


Compilacion:

    Para compilar, basta con ejecutar:
        make  
    O tambien:
        make all
    Esto creará tres archivos ejecutables: solve.out, vista.out y slave.out. Para eliminarlos ejecute make clean dentro del mismo directorio donde fueron creados.

Ejecucion:

    Para la ejecución es necesario contar con archivos de extensión .cnf que recibirá el programa solve.
    Para esto utilice el comando:
        ./solve.out files/*
    El programa enviará por salida estándar la cantidad de tareas a procesar. Este valor es utilizado por el programa vista. 
    Hay dos maneras de ejecutar el programa:
        1. Pasarle el resultado del solve al vista mediante un pipe.
            ./solve.out files/* | ./vista.out
        2. Ejecutar los procesos en dos terminales distintas.
            Terminal 1:
            ./solve.out files/*
            cantArchivos

            Terminal 2:
            ./vista.out cantArchivos

    Una vez que el programa solve termine, se creará un archivo output.txt con los resultados. Esto es indiferente si se ejecuta el vista o no.

Testing:

    Para realizar un testeo con valgrind, es necesario indicar en el Makefile el directorio donde se encuentran los archivos .cnf a analizar. Para ello, basta con asignar a la variable TESTF ese directorio.
    Luego, para realizar los testeos puede utilizar el siguiente comando:
        make test
    Los resultados se encontrarán en los siguientes archivos:
        - PVS-Studio: ...
        - Cppcheck: ...
        - Valgrind: ...
    Para eliminar estos archivos ejecute el siguiente comando en el directorio donde el comando test fue ejecutado:
        make clean_test 
    

