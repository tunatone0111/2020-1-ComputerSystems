#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include "myheader.h"
#include "mymacros.h"

char *heap_head, *pbrk, *free_list[3];
sem_t semid[HASHLEN+1];

int main(int argc,char* argv[]){
    char* file;
    clock_t start, end;
    size_t step, offset;
    char buffer[BUFF_SIZE];

    for(int i = 0; i<=HASHLEN; i++){
        sem_init(semid+i, 0, 1);
    }
    
    start = clock();

    pthread_t workers[NUM_THREADS];

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

        FILE *fp = fopen(file, "r");

        args_t arg[NUM_THREADS];

        for(int i = 0; i < NUM_THREADS; i++){
            arg[i].table = word_table;
            arg[i].fp = fp;
        }

        fseek(fp, 0, SEEK_END);
        size_t file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        step = file_size/NUM_THREADS;

        arg[0].startpoint = 0;
        arg[0].endpoint = step;

        for(int i = 1; i < NUM_THREADS; i++){
            fseek(fp, i*step, SEEK_SET);
            fgets(buffer, BUFF_SIZE, fp);
            offset = strlen(buffer);
            arg[i].startpoint = i*step + offset;
            arg[i-1].endpoint = i*step + offset;
        }
        arg[NUM_THREADS-1].endpoint = file_size;

        for(int i = 0; i < NUM_THREADS; i++){
            printf("T%d | start: %d, endpoint: %d\n", i, arg[i].startpoint, arg[i].endpoint);
            pthread_create(&workers[i], NULL, (void*)&Parser, (void*)(arg+i));
        }

        for(int i = 0; i < NUM_THREADS; i++)
            pthread_join(workers[i], NULL);

        fclose(fp);
    }

    end = clock();
    printf("evaulated time: %f\n", (double)(end-start));

    // traverse
    TableTraverse(word_table);

    exit(0);
}