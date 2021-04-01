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

Ejecucion:

    Primero es necesario ejecutar el archivo solve con los .cnf correspondientes, para lo cual puede utilizar el siguiente comando:
        ./solve.out files/*

Testing:

    Para realizar un testeo con valgrind, es necesario indicar en el Makefile el directorio donde se encuentran los archivos .cnf a analizar. Para ello, basta con asignar a la variable TESTF ese directorio.
    Luego, para realizar los testeos puede utilizar el siguiente comando:
        make test
