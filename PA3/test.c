#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "mymacros.h"
#include "myheader.h"

char *heap_head, *pbrk, *free_list[3];

void MyPrint(void* p, int n){
    unsigned char *temp = (unsigned char*)p;
    printf("%p: ", temp);
    for(int i = 0; i<n; i++){
        printf("%.2x ", temp[i]);
    }
    printf("\n");
}

int main(){

    HeapInitialization();

    getchar();
    int *pa = (int*)MyMalloc(sizeof(int));
    size_t *pb = (size_t*)MyMalloc(10*sizeof(size_t));
    getchar();
    *pa = 1;
    pb[0] = 2;
    for(int i = 0; i<10; i++)
        MyPrint(heap_head+WSIZE*i, WSIZE);
    printf("...\n");
    for(int i = 0; i<10; i++)
        MyPrint(heap_head+1024*1024*DSIZE+WSIZE*i, WSIZE);
    getchar();
    printf("%p: a = %d\n", pa, *pa);
    printf("%p: b = %ld\n", pb, *pb);
    getchar();
    MyFree(pa);
    MyFree(pb);

    for(int i = 0; i<10; i++)
        MyPrint(heap_head+WSIZE*i, WSIZE);
    for(int i = 0; i<10; i++)
        MyPrint(heap_head+1024*1024*DSIZE+WSIZE*i, WSIZE);

    exit(0);
}