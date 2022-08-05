#pragma once
#define S_INT_T 0

typedef struct s_keyl_t{
    struct s_keyl_t* next;
    unsigned int type;
    unsigned int key;
    void* data;
} s_keyl_t;

void shader_input_push(s_keyl_t** keys, unsigned int type, unsigned int key, void* data);
unsigned int shader_make(const char* vertex, const char* fragment);
unsigned int shader_make_from_file(const char* path, s_keyl_t* keys);