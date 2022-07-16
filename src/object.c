#include "object.h"

#include <cglm/cglm.h>

object_t obj_make(unsigned int mesh, unsigned int shader, unsigned int triangle_count){
    object_t obj;
    obj.triangle_count = triangle_count;
    obj.mesh = mesh;
    obj.shader = shader;

    obj.trans = malloc(16 * sizeof(float));
    glm_mat4_identity(*(mat4*)obj.trans);

    obj.color = malloc(3 * sizeof(float));
    obj.color[0] = 1.0f;
    obj.color[1] = 1.0f;
    obj.color[2] = 1.0f;
    return obj;
}

void obj_set_color(object_t *obj, float r, float g, float b){
    obj->color[0] = r;
    obj->color[1] = g;
    obj->color[2] = b;
}

void obj_transform(object_t *obj, 
    float tx, float ty, float tz, 
    float rx, float ry, float rz, float r,
    float sx, float sy, float sz)
{    
    glm_translate(obj->trans, (vec3){tx, ty, tz});
    glm_rotate(obj->trans, glm_deg(r), (vec3){rx, ry, rz});
    glm_scale(obj->trans, (vec3){sx, sy, sz});
}