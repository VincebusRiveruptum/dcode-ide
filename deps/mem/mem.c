#include "mem.h"

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

void *mem_create_arena(char *name, size_t size){
    MemoryArena *newArena = (MemoryArena *)malloc(sizeof(MemoryArena));
    if (!newArena) {
        logger("[mem_create_arena]: Could not allocate MemoryArena header");
        return NULL;
    }
    mem_arena_init(newArena, name, size);
    return newArena;
}

void mem_arena_init(MemoryArena *arena, char *name, size_t size){
    if(!arena) return;

    arena->base = malloc(size);
    if(!arena->base){
        logger("\n[mem_arena_init]: Error: Could not allocate %d bytes for arena %s", size, name);
        return;
    }

    arena->size = size;
    arena->offset = 0;
    strncpy(arena->name, name, 31);
    
    logger("\n[mem_arena_init]: Arena %s initialized with %d bytes", name, size);
}

// Not the best approach, but if someon wants to directly asign to an already
// avaliable arena object in the scope, they just pass the pointer to 
// the first parameter, 
// If is not in the same scope, you pass the name of the arena and the function itself
// will search for it

void *mem_arena_alloc(MemoryArena *arenaPtr, size_t size){
    void *ptr = NULL;

    if(!arenaPtr){
        logger("\n[mem_arenaPtr_alloc]: Error: Arena not found");
        return NULL;
    }

    if(!arenaPtr->base){
        logger("\n[mem_arenaPtr_alloc]: Error: Arena %s base is not allocated", arenaPtr->name);
        return NULL;
    }

    /* Align to 16 bytes for 64-bit/SSE safety on Linux and DOS */
    size = (size + 15) & ~15;

    if(arenaPtr->offset + size > arenaPtr->size){
        logger("\n[mem_arenaPtr_alloc]: Error: Arena %s out of memory (request: %d, left: %d)", 
               arenaPtr->name, size, arenaPtr->size - arenaPtr->offset);
        return NULL;
    }

    ptr = (void *)((unsigned char *)arenaPtr->base + arenaPtr->offset);
    arenaPtr->offset += size;

    return ptr;
}

void mem_arena_reset(MemoryArena *arenaPtr){
	if(!arenaPtr || !arenaPtr->base){
		logger("[mem_arena_reset]: No arena found!");
		return;
	}
    
    arenaPtr->offset = 0;
	return;
}

void mem_arena_free(MemoryArena *arenaPtr){
    if(!arenaPtr){
		logger("[mem_arena_free]: No arena found!");
		return;
	}

	logger("\n[mem_arena_free]: Freeing arena %s", arenaPtr->name);
	if(arenaPtr->base){
	    free(arenaPtr->base);
	    arenaPtr->base = NULL;
	}
	arenaPtr->offset = 0;
	arenaPtr->size = 0;
	free(arenaPtr);
	
	return;
}

// TODO : IMPROVE THIS FUNCTION ASAP
// DUMMY FOR NOW
size_t mem_getFileClosestSize(FILE *fp){
	size_t size = 0;
	if(!fp)
		return 0;

	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	rewind(fp);

	size += MEM_ARENA_8K; // for now...

	return size;
}