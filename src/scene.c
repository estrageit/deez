#include "scene.h"

#include <stdio.h>
#include <stdlib.h>

#include <glad/gl.h>
#include <cglm/cglm.h>

#include "renderer.h"
//#include "collider.h"
#include "shader.h"
#include "texture.h"
#include "mesh.h"
#include "object.h"

scene_t scene_load(const char* path){
    scene_t scene;
    unsigned int shaders[] = {
        shader_make_from_file("res/shaders/simple.glsl")
    };
    unsigned int i_counts[1];
    unsigned int vaos[] = {
        mesh_make("res/meshes/cube.json", &(i_counts[0]))
    };
    unsigned int uniforms[] = {
        glGetUniformLocation(shaders[0], "u_proj"),
        glGetUniformLocation(shaders[0], "u_view"),
        glGetUniformLocation(shaders[0], "u_model"),
        glGetUniformLocation(shaders[0], "u_color")
    };
    scene.objc = 4;
    scene.obj = malloc(scene.objc * sizeof(object_t));
    ((object_t*)scene.obj)[0] = obj_make(vaos[0], shaders[0], i_counts[0]);
    obj_set_color((object_t*)scene.obj + 0, 1.0f, 0.0f, 0.0f);
    obj_transform((object_t*)scene.obj + 0, 
        4.0f, 0.0f,-5.0f, 
        0.0f, 1.0f, 0.0f, 30.0f, 
        1.0f, 1.0f, 1.0f);

    ((object_t*)scene.obj)[1] = obj_make(vaos[0], shaders[0], i_counts[0]);
    obj_set_color((object_t*)scene.obj + 1, 0.0f, 1.0f, 0.0f);
    obj_transform((object_t*)scene.obj + 1, 
        -3.5f, 1.0f,-6.0f, 
        0.0f, 1.0f, 0.0f, 45.0f, 
        1.0f, 3.0f, 1.0f);

    ((object_t*)scene.obj)[2] = obj_make(vaos[0], shaders[0], i_counts[0]);
    obj_set_color((object_t*)scene.obj + 2, 0.0f, 0.0f, 1.0f);
    obj_transform((object_t*)scene.obj + 2, 
        1.0f, 0.0f, 2.0f, 
        0.0f, 1.0f, 0.0f, 0.0f, 
        1.0f, 1.0f, 1.0f);

    ((object_t*)scene.obj)[3] = obj_make(vaos[0], shaders[0], i_counts[0]);
    obj_set_color((object_t*)scene.obj + 3, 1.0f, 1.0f, 1.0f);
    obj_transform((object_t*)scene.obj + 3, 
        0.0f, -0.505f, 0.0f, 
        0.0f, 1.0f, 0.0f, 0.0f, 
        15.0f, 0.01f, 15.0f);

    scene.camera = malloc(sizeof(mat4));
    scene.proj = malloc(sizeof(mat4));

    scene.rend = rend_make(0, 0, 0, (unif_v){0, 0}, (uintv){0, 0});
    for (int i = 0; i < scene.objc; i++){
        unif_t *unif = malloc(sizeof(unif_t) * 4);
        unif[0] = (unif_t){uniforms[0], UNIF_MAT4, (mat4*)scene.proj};
        unif[1] = (unif_t){uniforms[1], UNIF_MAT4, (mat4*)scene.camera};
        unif[2] = (unif_t){uniforms[2], UNIF_MAT4, ((object_t*)scene.obj)[i].trans};
        unif[3] = (unif_t){uniforms[3], UNIF_VEC3, ((object_t*)scene.obj)[i].color};

        unif_v uniforms = {4, unif};

        rend_push(
            scene.rend, 
            ((object_t*)scene.obj)[i].shader, 
            ((object_t*)scene.obj)[i].mesh,
            ((object_t*)scene.obj)[i].triangle_count, 
            uniforms,
            (uintv){0, 0}
        );
    }
    scene.coll = NULL;
    scene.time = 0;

    glm_mat4_identity(*(mat4*)scene.camera);
    glm_look((vec3){0, 0, 0}, (vec3){1, 0, 0}, (vec3){0, 1, 0}, *(mat4*)scene.camera);

    glm_mat4_identity(*(mat4*)scene.proj);

    return scene;
}

void scene_update(scene_t* scene, float time, int wwidth, int wheight){
    float delta = time - scene->time;
    scene->time = time;

    glm_rotate(((object_t*)scene->obj)[2].trans, delta, (vec3){0, 1, 0});
    glm_perspective(glm_rad(45.0f), ((float)wwidth)/wheight, 0.1f, 10000.0f, *(mat4*)scene->proj);
    //glm_look((vec3){0, 0, 0}, (vec3){sin(time), 0, cos(time)}, (vec3){0, 1, 0}, *(mat4*)scene->camera);
    glm_look((vec3){16, 7, 16}, (vec3){-1, -0.5f, -1}, (vec3){0, 1, 0}, *(mat4*)scene->camera);

    rend_draw((rend_t*)scene->rend);
}