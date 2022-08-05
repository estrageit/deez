#pragma once

#define UNIF_INT 0
#define UNIF_FLOAT 1
#define UNIF_VEC2 2
#define UNIF_MAT4 3
#define UNIF_VEC3 4

typedef struct unif_t{
    struct unif_t* next;
    int location;
    int type;
    void *data;
} unif_t;

typedef struct obj_r_t{
    struct obj_r_t* next;
    //void* obj;
    unif_t* uniforms;
} obj_r_t;

typedef struct mat_r_t{
    struct mat_r_t* next;
    void* mat;
    unif_t* uniforms;
    obj_r_t* objs;
} mat_r_t;

typedef struct vao_r_t{
    struct vao_r_t* next;
    unsigned int vao;
    unsigned int count;
    unif_t* uniforms;
    mat_r_t* mats;
} vao_r_t;

typedef struct rend_t{
    struct rend_t* next;
    unsigned int shader;
    unif_t* uniforms;
    vao_r_t* vaos;
} rend_t;

void unif_push(unif_t** head, int location, int type, void* data);
void rend_object_push(mat_r_t* renderer_material);
void rend_material_push(vao_r_t* vao_renderer, void* mat);
void rend_vao_push(rend_t* shader, unsigned int vao, unsigned int count);
void rend_shader_push(rend_t** head, unsigned int shader);

void rend_draw(rend_t *head);