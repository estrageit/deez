#pragma once

typedef struct mesh_t {
    float *vertices;
    unsigned int *indices;
    unsigned int isize;
    unsigned int vsize;
    unsigned int icount;
} mesh_t;

unsigned int mesh_make(const char* path);