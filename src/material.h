#pragma once

typedef struct material_t {
    unsigned int diffuse;
    unsigned int specular;
    float shininess;
} material_t;

material_t* mat_make_from_file(void* idcache, const char* path);