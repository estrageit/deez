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
#include "input.h"

scene_t* scene_load(const char* path){
    scene_t* scene = malloc(sizeof(scene_t));
    unsigned int shaders[] = {
        shader_make_from_file("res/shaders/simple.glsl"),
        shader_make_from_file("res/shaders/basic.glsl")
    };
    unsigned int i_counts[1];
    unsigned int vaos[] = {
        mesh_make("res/meshes/cube.json", &(i_counts[0]))
    };
    unsigned int uniforms[] = {
        glGetUniformLocation(shaders[0], "u_proj"),
        glGetUniformLocation(shaders[0], "u_view"),
        glGetUniformLocation(shaders[0], "u_model"),
        glGetUniformLocation(shaders[0], "u_camera"),

        glGetUniformLocation(shaders[0], "u_mat.ambient"),
        glGetUniformLocation(shaders[0], "u_mat.diffuse"),
        glGetUniformLocation(shaders[0], "u_mat.specular"),
        glGetUniformLocation(shaders[0], "u_mat.shininess"),

        glGetUniformLocation(shaders[0], "u_light"),

        glGetUniformLocation(shaders[1], "u_proj"),
        glGetUniformLocation(shaders[1], "u_view"),
        glGetUniformLocation(shaders[1], "u_model"),
    };
    material_t* mats[] = {
        mat_make(1.0f, 1.0f, 1.0f, 0.5f, 32.0f),
        mat_make(0.0f, 1.0f, 1.0f, 1.0f, 8.0f),
        mat_make(1.0f, 0.0f, 1.0f, 0.5f, 2.0f),
        mat_make(1.0f, 1.0f, 0.0f, 0.5f, 32.0f),
        mat_make(1.0f, 0.0f, 1.0f, 1.0f, 16.0f)
    };
    scene->objc = 5 + 40;
    scene->obj = malloc(scene->objc * sizeof(object_t*));
    ((object_t**)scene->obj)[0] = obj_make(vaos[0], shaders[0], i_counts[0], mats[0]);
    obj_transform(*((object_t**)scene->obj + 0), 
        4.0f, 0.0f,-5.0f, 
        0.0f, 1.0f, 0.0f, 30.0f, 
        1.0f, 1.0f, 1.0f);

    ((object_t**)scene->obj)[1] = obj_make(vaos[0], shaders[0], i_counts[0], mats[1]);
    obj_transform(*((object_t**)scene->obj + 1), 
        -3.5f, 1.0f,-6.0f, 
        0.0f, 1.0f, 0.0f, 45.0f, 
        1.0f, 3.0f, 1.0f);

    ((object_t**)scene->obj)[2] = obj_make(vaos[0], shaders[0], i_counts[0], mats[2]);
    obj_transform(*((object_t**)scene->obj + 2), 
        1.0f, 0.0f, 2.0f, 
        0.0f, 1.0f, 0.0f, 0.0f, 
        1.0f, 1.0f, 1.0f);

    ((object_t**)scene->obj)[3] = obj_make(vaos[0], shaders[0], i_counts[0], mats[3]);
    obj_transform(*((object_t**)scene->obj + 3), 
        0.0f, -0.505f, 0.0f, 
        0.0f, 1.0f, 0.0f, 0.0f, 
        15.0f, 0.01f, 15.0f);
    
    ((object_t**)scene->obj)[4] = obj_make(vaos[0], shaders[0], i_counts[0], mats[4]);
    obj_transform(*((object_t**)scene->obj + 4), 
        0.0f, -0.505f, 0.0f, 
        0.0f, 1.0f, 0.0f, 0.0f, 
        15.0f, 0.01f, 15.0f);
    for (int i = 0; i < 40; i++){ //40
        float x, z;
        if (i < 10){
            x = -7.5f + i * (15.0f / 10);
            z = -7.5f;
        } else if(i < 20){
            x = -7.5f;
            z = -7.5f + (i - 10) * (15.0f / 10);
        } else if(i < 30){
            x =  7.5f;
            z = -7.5f + (i - 20) * (15.0f / 10);
        } else {
            x = -7.5f + (i - 30) * (15.0f / 10);
            z =  7.5f;
        }

        ((object_t**)scene->obj)[i + 5] = obj_make(vaos[0], shaders[0], i_counts[0], mats[4]);
        obj_transform(*((object_t**)scene->obj + 5 + i), 
            x, 1.0f, z, 
            0.0f, 1.0f, 0.0f, 0.0f, 
            1.0f, 3.0f, 1.0f);
    }
    
    scene->cam.trans = malloc(sizeof(mat4));
    scene->proj = malloc(sizeof(mat4));

    glm_vec3_copy((vec3){10, 10, 15}, scene->light_pos);

    scene->rend = rend_make(0, 0, 0, (unif_v){0, 0}, (uintv){0, 0});
    for (int i = 0; i < scene->objc; i++){
        unif_t *unif = malloc(sizeof(unif_t) * 9);
        unif[0] = (unif_t){uniforms[0], UNIF_MAT4, (mat4*)scene->proj};
        unif[1] = (unif_t){uniforms[1], UNIF_MAT4, (mat4*)scene->cam.trans};
        unif[2] = (unif_t){uniforms[2], UNIF_MAT4, ((object_t**)scene->obj)[i]->trans};
        unif[3] = (unif_t){uniforms[3], UNIF_VEC3, (vec3*)scene->cam.pos};

        unif[4] = (unif_t){uniforms[4], UNIF_VEC3, &((object_t**)scene->obj)[i]->mat->ambient};
        unif[5] = (unif_t){uniforms[5], UNIF_VEC3, &((object_t**)scene->obj)[i]->mat->diffuse};
        unif[6] = (unif_t){uniforms[6], UNIF_VEC3, &((object_t**)scene->obj)[i]->mat->specular};
        unif[7] = (unif_t){uniforms[7], UNIF_FLOAT, &((object_t**)scene->obj)[i]->mat->shininess};

        unif[8] = (unif_t){uniforms[8], UNIF_VEC3, &scene->light_pos};

        unif_v uni = {9, unif};

        rend_push(
            scene->rend, 
            ((object_t**)scene->obj)[i]->shader, 
            ((object_t**)scene->obj)[i]->mesh,
            ((object_t**)scene->obj)[i]->triangle_count, 
            uni,
            (uintv){0, 0}
        );
    }

    scene->light_trans = malloc(sizeof(mat4));
    glm_mat4_identity(*(mat4*)scene->light_trans);

    unif_t *unif = malloc(sizeof(unif_t) * 3);
    unif[0] = (unif_t){uniforms[9], UNIF_MAT4, (mat4*)scene->proj};
    unif[1] = (unif_t){uniforms[10], UNIF_MAT4, (mat4*)scene->cam.trans};
    unif[2] = (unif_t){uniforms[11], UNIF_MAT4, (mat4*)scene->light_trans};
    unif_v uni = {3, unif};
    rend_push(
        scene->rend, 
        shaders[1], 
        vaos[0],
        36, 
        uni,
        (uintv){0, 0}
    );

    scene->coll = NULL;
    scene->time = 0;

    glm_vec3_zero(*(vec3*)scene->cam.pos);
    glm_vec3_zero(*(vec3*)scene->cam.rot);
    scene->cam.rot[2] = 1;
    camera_transform(&scene->cam);
    glm_mat4_identity(*(mat4*)scene->cam.trans);
    glm_mat4_identity(*(mat4*)scene->proj);

    return scene;
}

void camera_transform(camera_t* cam){
    glm_look(*(vec3*)cam->pos, *(vec3*)cam->rot, (vec3){0, 1, 0}, *(mat4*)cam->trans);
}

void scene_update(scene_t* scene, float time, int wwidth, int wheight, void* input){
    float delta = time - scene->time;
    scene->time = time;

    vec3 mov = {0, 0, 0};
    vec3 rot = {((input_t*)input)->m.x, -((input_t*)input)->m.y, 0};
    mat4 trans;

    glm_look((vec3){0, 0, 0}, scene->cam.rot, (vec3){0, 1, 0}, trans);
    glm_mat4_inv(trans, trans);

    if (((input_t*)input)->k.k & (uint64_t)1 << ('W' - 32))
        glm_vec3_add((vec3){ 0, 0, 1}, mov, mov);
    if (((input_t*)input)->k.k & (uint64_t)1 << ('S' - 32))
        glm_vec3_add((vec3){ 0, 0,-1}, mov, mov);
    if (((input_t*)input)->k.k & (uint64_t)1 << ('D' - 32))
        glm_vec3_add((vec3){-1, 0, 0}, mov, mov);
    if (((input_t*)input)->k.k & (uint64_t)1 << ('A' - 32))
        glm_vec3_add((vec3){ 1, 0, 0}, mov, mov);

    glm_vec3_rotate_m4(trans, rot, rot);
    glm_vec3_add(scene->cam.rot, rot, rot);
    glm_vec3_normalize(rot);
    if (glm_vec3_angle((vec3){0, 1, 0}, rot) > 0.1f &&
        glm_vec3_angle((vec3){0, -1, 0}, rot) > 0.1f){
        glm_vec3_copy(rot, scene->cam.rot);
    }

    glm_vec3_normalize(mov);
    glm_vec3_scale(mov, -2 * delta, mov);
    glm_vec3_rotate_m4(trans, mov, mov);
    glm_vec3_add(scene->cam.pos, mov, scene->cam.pos);

    camera_transform(&scene->cam);

    glm_perspective(glm_rad(90.0f), ((float)wwidth)/wheight, 0.1f, 1000.0f, *(mat4*)scene->proj);
    glm_vec3_copy((vec3){sin(time) * 10, 10, cos(time) * 10}, scene->light_pos);

    glm_mat4_identity(*(mat4*)scene->light_trans);
    glm_translate(scene->light_trans, scene->light_pos);

    rend_draw((rend_t*)scene->rend);
}