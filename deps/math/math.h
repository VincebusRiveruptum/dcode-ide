#ifndef MATH_H
#define MATH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

/* Coordinate system */
typedef struct Coordinates {
    long x;     /* 16.16 Fixed Point */
    long y;     /* 16.16 Fixed Point */
    int z;
} Coordinates;

typedef struct ScreenCoordinates {
    int x;
    int y;
} ScreenCoordinates;

/* Transformations */
typedef struct RotationTransformation {
    int angle;
    int current;
} RotationTransformation;

typedef struct TranslationTransformation {
    struct Coordinates *dest;
    unsigned char loop;
} TranslationTransformation;

typedef struct Transformation {
    unsigned char type;
    void *data;
} Transformation;

/* CONSTANTS =============================================================== */

#define PI 3.14159265358979323846
#define DEG2RAD (PI / 180.0)

/* PROTOTYPES ============================================================== */

int m_round(float x);

/* GLOBAL VARS ============================================================= */

extern long m_sintable[360];
extern long m_costable[360];

#endif
