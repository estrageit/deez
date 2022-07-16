#pragma once

typedef struct object_t {
    unsigned int mesh;
    unsigned int shader;
    unsigned int triangle_count; // make sure this is gone
    void *trans;
    float *color;
} object_t;

object_t obj_make(unsigned int mesh, unsigned int shader, unsigned int triangle_count);
void obj_set_color(object_t *obj, float r, float g, float b);
void obj_transform(object_t *obj, 
    float tx, float ty, float tz, 
    float r, float rx, float ry, float rz, 
    float sx, float sy, float sz);