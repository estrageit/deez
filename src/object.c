#include "object.h"

#include <cglm/cglm.h>
#include <stdio.h>

material_t* mat_make(float r, float g, float b, float specular, float shine){
    material_t* mat = malloc(sizeof(material_t));

    mat->ambient[0] = r;
    mat->ambient[1] = g;
    mat->ambient[2] = b;

    mat->diffuse[0] = r;
    mat->diffuse[1] = g;
    mat->diffuse[2] = b;

    mat->specular[0] = specular;
    mat->specular[1] = specular;
    mat->specular[2] = specular;

    mat->shininess = shine;
    return mat;
}

object_t* obj_make(unsigned int mesh, unsigned int shader, unsigned int triangle_count, material_t* material){    
    object_t* obj = malloc(sizeof(object_t));

    obj->triangle_count = triangle_count;
    obj->mesh = mesh;
    obj->shader = shader;
    obj->mat = material;

    obj->trans = malloc(sizeof(mat4));
    glm_mat4_identity(obj->trans);
    return obj;
}

void obj_transform(object_t *obj, 
    float tx, float ty, float tz, 
    float rx, float ry, float rz, float r,
    float sx, float sy, float sz)
{    
    glm_mat4_identity(obj->trans);
    glm_translate(*(mat4*)obj->trans, (vec3){tx, ty, tz});
    glm_rotate(*(mat4*)obj->trans, glm_deg(r), (vec3){rx, ry, rz});
    glm_scale(*(mat4*)obj->trans, (vec3){sx, sy, sz});
}