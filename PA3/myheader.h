#ifndef MYHEAD
#include <stddef.h>
#include <stdio.h>
#define MYHEAD
#define HASHLEN 64*1024
#define NUM_THREADS 1
#define WORDSIZE 8
#define BUFF_SIZE 256

typedef unsigned int pointer;

typedef struct _word{
    int count;
    struct _word *next;
    char val[];
}word_t;

typedef struct _arguments{
    word_t* *table;
    FILE* fp;
    int startpoint;
    int endpoint;
}args_t;

void Parser(void*);
size_t HashFunc(char*);
void TableTraverse(word_t*[]);
word_t* IsInTab(word_t*[], char*);
void WordCount(word_t*[], char*);
void WordAdd(word_t*[], char*);

void HeapInitialization();
void* MyMalloc(size_t);
void MyFree(void*);

#endif