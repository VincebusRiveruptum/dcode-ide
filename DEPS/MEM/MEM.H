
#ifndef MEM_H
#define MEM_H

#include "STD.H"
#include "../LOG/LOG.H"

#define MAX_ARENAS 32

/* ARENA SIZES =========================================================================*/

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

/* Arena types , could be useful for other modules*/

#define MEM_ARENA_DEFAULT 0
#define MEM_ARENA_MAIN 1
#define MEM_ARENA_FILE 2
#define MEM_ARENA_TEST 3

/* TYPES ===========================================================================*/

typedef struct MemoryArena{
    char name[32];
    unsigned char type;
    void *base;
    size_t size;
    size_t offset;
} MemoryArena;

/* GLOBAL VARS ===========================================================================*/
extern MemoryArena memoryArenas[MAX_ARENAS];

/* PROTOTYPES ===========================================================================*/

void mem_init();
void mem_shutdown();

void mem_arena_init(MemoryArena *arena, char *name, unsigned char type, size_t size);
void *mem_create_arena(char *name, unsigned char type, size_t size);
void *mem_arena_alloc(MemoryArena *arenaPtr, char *name, size_t size);
MemoryArena *mem_get_arena(char *name);
void mem_arena_reset(char *name);
void mem_arena_free(char *name);

#endif