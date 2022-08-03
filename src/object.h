#pragma once

typedef struct material_t {
    float specular[3];
    float diffuse[3];
    float ambient[3];
    float shininess;
    unsigned int texture;
} material_t;

typedef struct object_t {
    unsigned int mesh;
    unsigned int shader;
    unsigned int triangle_count; // make sure this is gone

    material_t* mat;
    void* trans;
} object_t;

material_t* mat_make(float r, float g, float b, float specular, float shine);
material_t* mat_make_from_file(void* idcache, const char* path);

object_t* obj_make(unsigned int mesh, unsigned int shader, unsigned int triangle_count, material_t* material);
void obj_transform(object_t *obj, 
    float tx, float ty, float tz, 
    float r, float rx, float ry, float rz, 
    float sx, float sy, float sz);