#pragma once

typedef struct mesh_t{
    unsigned int vao;
    unsigned int tcount;
} mesh_t;

mesh_t* mesh_make(const char* path);