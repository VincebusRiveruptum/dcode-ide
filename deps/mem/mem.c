#include "mem.h"

void _null_arena(char *name);

/* Now it iwll handle dynamic arenas  */

/*
    Instructions:

    1. First before everything, you need to call mem_init()
    2. Then you need to call mem_create_arena() to create an arena
    3. Then you can use mem_arena_alloc() to allocate memory
    4. Then you can use mem_arena_reset() to reset the arena
    5. Then you can use mem_arena_free() to free the arena
    6. Then you can use mem_shutdown() to shutdown the arena

*/ 

MemoryArena memoryArenas[MAX_ARENAS];

void mem_init() {
    memset(memoryArenas, 0, sizeof(memoryArenas));
}

void mem_shutdown() {
    int i=0;

    for(i=0; i<MAX_ARENAS; i++){
        if(memoryArenas[i].name[0] != '\0'){
            mem_arena_free(&memoryArenas[i], NULL);
        }
    }
}

void mem_arena_init(MemoryArena *arena, char *name, unsigned char type, size_t size){
    if(!arena) return;

    arena->base = malloc(size);
    if(!arena->base){
        logger("\n[mem_arena_init]: Error: Could not allocate %d bytes for arena %s", size, name);
        return;
    }

    arena->size = size;
    arena->offset = 0;
    arena->type = type;
    strncpy(arena->name, name, 31);
    
    logger("\n[mem_arena_init]: Arena %s initialized with %d bytes", name, size);
}

void *mem_create_arena(char *name, unsigned char type, size_t size){
    int i=0;
    for(i=0; i<MAX_ARENAS; i++){
        if(memoryArenas[i].name[0] == '\0'){
            mem_arena_init(&memoryArenas[i], name, type, size);
            return &memoryArenas[i];
        }
    }
    return NULL;
}

// Not the best approach, but if someon wants to directly asign to an already
// avaliable arena object in the scope, they just pass the pointer to 
// the first parameter, 
// If is not in the same scope, you pass the name of the arena and the function itself
// will search for it
void *mem_arena_alloc(MemoryArena *arenaPtr, char *name, size_t size){
    void *ptr = NULL;
    MemoryArena *arena = NULL;

    arena = arenaPtr;

    if(!arena && !name){
        logger("\n[mem_arena_alloc]: Error: Arena pointer and name are null!");
        return NULL;
    }

    if(!arena && name){
        arena = mem_get_arena(name);
    }
    
    if(!arena){
        logger("\n[mem_arena_alloc]: Error: Arena %s not found", name);
        return NULL;
    }

    if(!arena->base){
        logger("\n[mem_arena_alloc]: Error: Arena %s base is not allocated", arena->name);
        return NULL;
    }

    // Align to 4 bytes for 32-bit performance
    size = (size + 3) & ~3;

    if(arena->offset + size > arena->size){
        logger("\n[mem_arena_alloc]: Error: Arena %s out of memory (request: %d, left: %d)", 
               arena->name, size, arena->size - arena->offset);
        return NULL;
    }

    ptr = (void *)((unsigned char *)arena->base + arena->offset);
    arena->offset += size;

    return ptr;
}

MemoryArena *mem_get_arena(char *name){
    int i=0;
    for(i=0; i<MAX_ARENAS; i++){
        if(!strcmp(memoryArenas[i].name, name)){
            return &memoryArenas[i];
        }
    }
    return NULL;
}

void mem_arena_reset(char *name){
    MemoryArena *arena = NULL;
    arena = mem_get_arena(name);
    if(arena){
        arena->offset = 0;
    }
}

void mem_arena_free(MemoryArena *arenaPtr,char *name){
    MemoryArena *arena = NULL;

    if(!arenaPtr){
        arena = mem_get_arena(name);
    }else{
        arena = arenaPtr;
    }

    if(arena && arena->base){
        logger("\n[mem_arena_free]: Freeing arena %s", arena->name);
        free(arena->base);
        arena->base = NULL;
        arena->offset = 0;
        arena->size = 0;
    }

    if(name == NULL && arena != NULL){
        name = arena->name;
    }

    _null_arena(name);
}

void _null_arena(char *name){
    int i = 0;
    if (!name) return;
    for(i=0; i<MAX_ARENAS; i++){
        if(!strcmp(memoryArenas[i].name, name)){
            memoryArenas[i].base = NULL;
            memoryArenas[i].offset = 0;
            memoryArenas[i].size = 0;
            memoryArenas[i].type = 0;
            memset(memoryArenas[i].name, 0, 32);
        }
    }    
}

/* Helper functions */

int get_arena_count(){
    int i=0;
    for(i=0; i<MAX_ARENAS; i++){
        if(memoryArenas[i].name[0] != '\0'){
            i++;
        }
    }
    return i;
}

