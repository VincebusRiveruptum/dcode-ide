#include "imgfile.h"

BMPfile *loadBMPfile(char *fileName, MemoryArena *arena){
    int padding = 0;
    int y = 0;
    FILE *fp = NULL;
    BMPfile *newFile = NULL;
    char *id = NULL;

    if (!fileName || !arena) {
        return NULL;
    }

    fp = fopen(fileName, "rb");

    if (!fp) {
        logger("[loadBMPfile]: Error, file not found!");
        return NULL;
    }

    logger("[loadBMPfile]: Loading %s ", fileName);

    id = (char *)calloc(3, sizeof(char));
    if (!id) {
        fclose(fp);
        return NULL;
    }

    newFile = (BMPfile *)mem_arena_alloc(
        arena,
        sizeof(BMPfile)
    );
    if (!newFile) {
        free(id);
        fclose(fp);
        return NULL;
    }

    newFile->bmpData = (BMPdata *)mem_arena_alloc(
        arena,
        sizeof(BMPdata)
    );
    if (!newFile->bmpData) {
        free(id);
        fclose(fp);
        return NULL;
    }

    newFile->bmpData->bmp = NULL;
    newFile->bmpData->palette = (Color *)mem_arena_alloc(
        arena,
        256 * sizeof(Color)
    );

    if (newFile->bmpData->palette == NULL) {
        logger("[loadBMPfile]: Memory allocation failed");
        free(id);
        fclose(fp);
        return NULL;
    }
    memset(newFile->bmpData->palette, 0, 256 * sizeof(Color));

    if (fread(id, 2, 1, fp) != 1) {
        logger("[loadBMPfile]: Failed reading magic header");
        free(id);
        fclose(fp);
        return NULL;
    }

    if (strcmp(id, "BM") != 0) {
        logger("[loadBMPfile]: Invalid file. %s", id);
        free(id);
        fclose(fp);
        return NULL;
    }

    memcpy(newFile->fh.id, id, 2);
    free(id);
    id = NULL;

    if (fread(&(newFile->fh.size), 12, 1, fp) != 1) {
        fclose(fp);
        return NULL;
    }
    if (fread(&(newFile->ih), 40, 1, fp) != 1) {
        fclose(fp);
        return NULL;
    }

    logger(
        "[loadBMPfile]: %s [ X : %ld, Y : %ld ]",
        fileName,
        newFile->ih.x,
        newFile->ih.y
    );

    newFile->bmpData->width = newFile->ih.x;
    newFile->bmpData->height = newFile->ih.y;

    /* Read Palette */
    if (fread(newFile->bmpData->palette, 1024, 1, fp) != 1) {
        fclose(fp);
        return NULL;
    }

    /* Read Bitmap Image */
    newFile->bmpData->bmp = (unsigned char **)mem_arena_alloc(
        arena,
        sizeof(unsigned char *) * newFile->ih.y
    );

    if (newFile->bmpData->bmp == NULL) {
        logger("[loadBMPfile]: Could not allocate bmp height.");
        fclose(fp);
        return NULL;
    }

    while ((newFile->ih.x + padding) % 4 != 0) {
        padding++;
    }

    for (y = (int)newFile->ih.y - 1; y >= 0; y--) {
        newFile->bmpData->bmp[y] = (unsigned char *)mem_arena_alloc(
            arena,
            sizeof(unsigned char) * (newFile->ih.x + padding)
        );

        if (newFile->bmpData->bmp[y] == NULL) {
            logger(
                "[loadBMPfile]: Could not allocate width on index %d",
                y
            );
            fclose(fp);
            return NULL;
        }

        if (fread(
                newFile->bmpData->bmp[y],
                newFile->ih.x + padding,
                1,
                fp
            ) != 1) {
            fclose(fp);
            return NULL;
        }
    }

    fclose(fp);
    return newFile;
}
