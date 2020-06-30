#include "myheader.h"
#include "mymacros.h"
#include <unistd.h>
#include <stdio.h>

extern char *heap_head, *pbrk, *free_list[2];

void HeapInitialization(){
    if((heap_head = (char*)sbrk(512*1024*2*DSIZE)) == (void*)-1)
        return;

    free_list[0] = heap_head + WSIZE;
    for(int i = 0; i < 512*1024; i++){
        PUT(HDRP(free_list[0]+i*2*DSIZE), 3*WSIZE);
        PUT(free_list[0]+i*2*DSIZE, (size_t)(free_list[0]+i*2*DSIZE+2*DSIZE));
    }
    PUT(free_list[0]+(512*1024-1)*2*DSIZE, 0);

    free_list[1] = sbrk(512*1024*3*DSIZE) + WSIZE;
    for(int i = 0; i < 512*1024; i++){
        PUT(HDRP(free_list[1]+i*3*DSIZE), 5*WSIZE);
        PUT(free_list[1]+i*3*DSIZE, (size_t)(free_list[1]+i*3*DSIZE+3*DSIZE));
    }
    PUT(free_list[1]+(512*1024-1)*3*DSIZE, 0);

    pbrk = (char*)sbrk(0);

    // printf("done. memory size: %ld\n", pbrk - heap_head);
    // printf("heap head: %p\n", heap_head);
    // printf("%p\n%p\n", free_list[0], free_list[1]);
}

void* MyMalloc(size_t size){
    int notfound = 0;
    char *temp, *r;
    size_t extend_size;

    if(size<1){
        r = NULL;
    }else if(size<=3*WSIZE){
        if(free_list[0] == NULL) notfound = 1;
        else{
            r = free_list[0];
            free_list[0] = NEXT_BLKP(free_list[0]);
        }
    }else if(size<=5*WSIZE){
        if(free_list[1] == NULL) notfound = 2;
        else{
            r = free_list[1];
            free_list[1] = NEXT_BLKP(free_list[1]);
        }
    }else{
        temp = free_list[2];
        if(temp == NULL) notfound = 3;
        else{
            while(GET_SIZE(temp) < size){
                if(NEXT_BLKP(temp) == NULL){
                    notfound = 3;
                    break;
                }
                temp = NEXT_BLKP(temp);
            }
            if(!notfound){
                r = temp;
                free_list[2] = NEXT_BLKP(temp);
            }
        }
    }

    switch(notfound){
        case 0:
            break;
        case 1:
            pbrk = sbrk(2*DSIZE) + WSIZE;
            PUT(HDRP(pbrk), 3*WSIZE);
            PUT(pbrk, 0);
            free_list[0] = pbrk;
        case 2:
            pbrk = sbrk(3*DSIZE) +WSIZE;
            PUT(HDRP(pbrk), 5*WSIZE);
            PUT(pbrk, 0);
            free_list[1] = pbrk;
        case 3:
            extend_size = size/WSIZE + 2;
            extend_size = extend_size%2 ? (extend_size + 1)*WSIZE : extend_size*WSIZE;
            pbrk = sbrk(extend_size) + WSIZE;
            PUT(HDRP(pbrk), extend_size-WSIZE);
            PUT(pbrk, 0);
            free_list[2] = pbrk;

        pbrk = sbrk(0);
        r = (char*)MyMalloc(size);
    }

    return r;
}

void MyFree(void* target){
    size_t size = GET_SIZE(target);
    switch(size){
        case 3*WSIZE:
            PUT(target, (size_t)(free_list[0]));
            free_list[0] = target;
            break;
        case 5*WSIZE:
            PUT(target, (size_t)(free_list[1]));
            free_list[1] = target;
            break;
        default:
            PUT(target, (size_t)(free_list[2]));
            free_list[2] = target;
    }

}