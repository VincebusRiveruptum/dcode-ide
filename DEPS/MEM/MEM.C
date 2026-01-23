#include "MEM.H"


// Global pointers
MemoryArena *editorArena = NULL;
MemoryArena *fileArena = NULL;
MemoryArena *testArena = NULL;

void mem_init() {
    editorArena = (MemoryArena*)malloc(sizeof(MemoryArena));
    fileArena = (MemoryArena*)malloc(sizeof(MemoryArena));
    testArena = (MemoryArena*)malloc(sizeof(MemoryArena));

    if(!editorArena || !fileArena || !testArena){
        printf("\n[mem_init]: FATAL ERROR: Could not allocate memory for arena control structures.");
        exit(1);
    }

    mem_arena_init(editorArena, "Editor", MEM_ARENA_PERMANENT, ARENA_SIZE_EDITOR);
    mem_arena_init(fileArena,   "File",   MEM_ARENA_CACHE,     ARENA_SIZE_FILE);
    mem_arena_init(testArena,   "Test",   MEM_ARENA_PURGABLE,  ARENA_SIZE_TEST);
}

void mem_shutdown() {
    mem_arena_free(editorArena);
    mem_arena_free(fileArena);
    mem_arena_free(testArena);
    
    free(editorArena);
    free(fileArena);
    free(testArena);
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

void *mem_arena_alloc(MemoryArena *arena, size_t size){
    void *ptr = NULL;

    if(!arena || !arena->base){
        logger("\n[mem_arena_alloc]: Error: Arena is not initialized");
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

void mem_arena_reset(MemoryArena *arena){
    if(arena){
        arena->offset = 0;
    }
}

void mem_arena_free(MemoryArena *arena){
    if(arena && arena->base){
        logger("\n[mem_arena_free]: Freeing arena %s", arena->name);
        free(arena->base);
        arena->base = NULL;
        arena->offset = 0;
        arena->size = 0;
    }
}
