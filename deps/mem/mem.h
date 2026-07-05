#ifndef MEM_H
#define MEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if defined(__MSDOS__) || defined(__WATCOMC__)
#include <conio.h>
#include <dos.h>    
#include <i86.h>
#endif

#include "../log/log.h"

/* ARENA SIZES =========================================================================*/

#define MEM_ARENA_1K 1024    // 1kb
#define MEM_ARENA_2K 1024 * 2 // 2kb
#define MEM_ARENA_4K 1024 * 4 // 4kb
#define MEM_ARENA_8K 1024 * 8 // 8kb
#define MEM_ARENA_16K 1024 * 16 // 16kb
#define MEM_ARENA_32K 1024 * 32 // 32kb
#define MEM_ARENA_64K 1024 * 64 // 64kb
#define MEM_ARENA_256K 1024 * 256 // 256 kb
#define MEM_ARENA_512K 1024 * 512 // 512 kb
#define MEM_ARENA_1M 1024 * 1024 // 1 mb
#define MEM_ARENA_2M 1024 * 1024 * 2 // 2 mb
#define MEM_ARENA_4M 1024 * 1024 * 4 // 4 mb
#define MEM_ARENA_8M 1024 * 1024 * 8 // 8 mb
#define MEM_ARENA_16M 1024 * 1024 * 16 // 16 mb

/* TYPES ===========================================================================*/

typedef struct MemoryArena{
    char name[32];
    void *base;
    size_t size;
    size_t offset;
} MemoryArena;

/* PROTOTYPES ===========================================================================*/

void mem_arena_init(MemoryArena *arenaPtr, char *name,size_t size);
void *mem_create_arena(char *name, size_t size);
void *mem_arena_alloc(MemoryArena *arenaPtr, size_t size);
void mem_arena_reset(MemoryArena *arenaPtr);
void mem_arena_free( MemoryArena *arenaPtr, char *name);

size_t mem_getFileClosestSize(FILE *fp);

#endif