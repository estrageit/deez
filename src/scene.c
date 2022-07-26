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
    scene->time = 0;
    scene->rend = NULL;
    glm_vec3_copy((vec3){0, 0, 1}, scene->camera_rot);

    unsigned int shader = shader_make_from_file("res/shaders/simple.glsl");
    rend_shader_push((rend_t**)&(scene->rend), shader);
    scene->proj = malloc(sizeof(mat4));
    scene->view = malloc(sizeof(mat4));
    glm_mat4_identity(scene->proj);
    glm_mat4_identity(scene->view);
    glm_vec3_zero(scene->camera_pos);
    glm_vec3_zero(scene->light_pos);
    unif_push(&(((rend_t*)scene->rend)->uniforms), glGetUniformLocation(shader, "u_proj"), UNIF_MAT4, scene->proj);
    unif_push(&(((rend_t*)scene->rend)->uniforms), glGetUniformLocation(shader, "u_view"), UNIF_MAT4, scene->view);
    unif_push(&(((rend_t*)scene->rend)->uniforms), glGetUniformLocation(shader, "u_camera"), UNIF_VEC3, scene->camera_pos);
    unif_push(&(((rend_t*)scene->rend)->uniforms), glGetUniformLocation(shader, "u_light"), UNIF_VEC3, scene->light_pos);

    unsigned int ib_count;
    unsigned int vao = mesh_make("res/meshes/cube.json", &ib_count);
    rend_vao_push(scene->rend, vao, ib_count);

    rend_material_push(((rend_t*)scene->rend)->vaos, 0);
    material_t* mat = mat_make(1, 0.5f, 1, 0.5f, 32.0f);
    unif_push(&(((rend_t*)scene->rend)->vaos->mats->uniforms), glGetUniformLocation(shader, "u_mat.ambient"), UNIF_VEC3, mat->ambient);
    unif_push(&(((rend_t*)scene->rend)->vaos->mats->uniforms), glGetUniformLocation(shader, "u_mat.diffuse"), UNIF_VEC3, mat->diffuse);
    unif_push(&(((rend_t*)scene->rend)->vaos->mats->uniforms), glGetUniformLocation(shader, "u_mat.specular"), UNIF_VEC3, mat->specular);
    unif_push(&(((rend_t*)scene->rend)->vaos->mats->uniforms), glGetUniformLocation(shader, "u_mat.shininess"), UNIF_FLOAT, &mat->shininess);

    rend_object_push(((rend_t*)scene->rend)->vaos->mats);
    scene->model = malloc(sizeof(mat4));
    glm_mat4_identity(scene->model);
    glm_translate(scene->model, (vec3){0, 0, 3});
    glm_rotate(scene->model, 45, (vec3){1, 1, 1});
    unif_push(&(((rend_t*)scene->rend)->vaos->mats->objs->uniforms), glGetUniformLocation(shader, "u_model"), UNIF_MAT4, scene->model);

    return scene;
}

void scene_update(scene_t* scene, float time, int wwidth, int wheight, void* input){
    float delta = time - scene->time;
    scene->time = time;

    vec3 mov = {0, 0, 0};
    vec3 rot = {((input_t*)input)->m.x, -((input_t*)input)->m.y, 0};
    mat4 trans;

    glm_look((vec3){0, 0, 0}, scene->camera_rot, (vec3){0, 1, 0}, trans);
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
    glm_vec3_add(scene->camera_rot, rot, rot);
    glm_vec3_normalize(rot);
    if (glm_vec3_angle((vec3){0, 1, 0}, rot) > 0.1f &&
        glm_vec3_angle((vec3){0, -1, 0}, rot) > 0.1f){
        glm_vec3_copy(rot, scene->camera_rot);
    }

    glm_vec3_normalize(mov);
    glm_vec3_scale(mov, -2 * delta, mov);
    glm_vec3_rotate_m4(trans, mov, mov);
    glm_vec3_add(scene->camera_pos, mov, scene->camera_pos);

    glm_look(scene->camera_pos, scene->camera_rot, (vec3){0, 1, 0}, scene->view);

    glm_perspective(glm_rad(90.0f), ((float)wwidth)/wheight, 0.1f, 1000.0f, *(mat4*)scene->proj);
    glm_vec3_copy((vec3){sin(time) * 10, 10, cos(time) * 10}, scene->light_pos);

    rend_draw(scene->rend);
}