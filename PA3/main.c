#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include "myheader.h"
#include "mymacros.h"

char *heap_head, *pbrk, *free_list[3];
// sem_t semid[HASHLEN];

int main(int argc,char* argv[]){
    char* file;
    clock_t start, end;

    // for(int i = 0; i<HASHLEN; i++){
    //     sem_init(semid+i, 0, 1);
    // }
    
    start = clock();

    // pthread_t workers[NUM_THREADS];

    // word table
    word_t *word_table[HASHLEN] = {0};

    // run
    if(argc<2){
        printf("please enter file name as argument\nex) ./main enwiki-latest-all-titles-in-ns0\n");
        exit(0);
    }else{
        // Initialize heap
        HeapInitialization();

        file = argv[1];
        printf("Targeted file: %s\n", file);

        // pthread_create(&workers[0], NULL, func, arg);

        FILE *fp = fopen(file, "r");

        Parser(word_table, fp);

        fclose(fp);
    }

    end = clock();
    printf("evaulated time: %f\n", (double)(end-start));

    // traverse
    TableTraverse(word_table);

    exit(0);
}