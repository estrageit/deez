#pragma once
#include "link.h"
#include "util.h"

#define UNIF_INT 0
#define UNIF_FLOAT 1
#define UNIF_VEC2 2
#define UNIF_MAT4 3
#define UNIF_VEC3 4

typedef struct unif_t{
    int location;
    int type;
    void *data;
} unif_t;

typedef struct unif_v{
    int size;
    unif_t *data;
} unif_v;

typedef struct rend_t{
    struct rend_t *next;
    unsigned int shader;
    unsigned int vao;
    unsigned int count;
    unif_v uniforms;
    uintv textures;
} rend_t;

rend_t *rend_make(unsigned int shader, unsigned int vao, unsigned int count, unif_v uniforms, uintv textures);
void rend_push(rend_t *head, unsigned int shader, unsigned int vao, unsigned int count, unif_v uniforms, uintv textures);
void rend_remove(rend_t *head, rend_t *node);
void rend_removeat(rend_t *head, unsigned int index);
void rend_destroy(rend_t *head);
void rend_draw(rend_t *head);