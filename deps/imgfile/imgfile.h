#ifndef IMGFILE_H
#define IMGFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../log/log.h"
#include "../mem/mem.h"

struct MemoryArena;

typedef struct FileHeader {
    unsigned char id[2];
    long size;
    int res1[2];
    long offset;	
} FileHeader;

typedef struct InfoHeader {
    long hsize;
    long x;
    long y;
    int numColorPlanes;
    int bitsPerPixel;
    long compressionMethod;
    long imgSize;
    long resX;
    long resY;
    long numColors;
    long numImportantColors;
} InfoHeader;

typedef struct Color {
    unsigned char b;
    unsigned char g;
    unsigned char r;
    unsigned char i;
} Color;

typedef struct BMPfile {
    struct FileHeader fh;
    struct InfoHeader ih;
    struct BMPdata *bmpData;
} BMPfile;

typedef struct BMPdata {
    unsigned char **bmp;
    struct Color *palette;
    long width;
    long height;
} BMPdata;

/* Prototypes ============================================= */

BMPfile *loadBMPfile(char *fileName, struct MemoryArena *arena);

#endif
