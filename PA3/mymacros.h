#ifndef MYMACRO
#define MYMACRO

#include <stddef.h>
#define WSIZE 8
#define DSIZE 16

// read and write a word at address p
#define GET(p)      (*(size_t *)(p))
#define PUT(p, val) (*(size_t *)(p) = (val))

// get header address of the block
#define HDRP(bp)    ((char*)(bp) - WSIZE)

#define GET_SIZE(bp) GET(HDRP(bp))

#define NEXT_BLKP(bp) ((char*)GET(bp))

#endif