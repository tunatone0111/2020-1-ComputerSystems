#include "myheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>

extern sem_t semid[HASHLEN];

void WordCount(word_t* table[], char* x){
    word_t *temp;
    int hash_val = HashFunc(x);
    sem_wait(semid+hash_val);
    if((temp = IsInTab(table, x))){
        temp->count = temp->count+1;
    }else WordAdd(table, x);
    sem_post(semid+hash_val);
}

void WordAdd(word_t* table[], char* x){
    sem_wait(semid+HASHLEN);
    word_t *newnode = (word_t*)MyMalloc(sizeof(word_t) + strlen(x) + 1);
    sem_post(semid+HASHLEN);
    newnode->count = 1;
    newnode->next = NULL;
    strcpy(newnode->val, x);

    word_t *temp = table[HashFunc(x)];
    if(temp == NULL) table[HashFunc(x)] = newnode;
    else{
        while(temp->next != NULL)
            temp = temp->next;
        temp->next = newnode;
    }
}