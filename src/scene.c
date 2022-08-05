#include "scene.h"

#include <stdio.h>
#include <stdlib.h>

#include <glad/gl.h>
#include <cglm/cglm.h>
#include <json-c/json.h>

#include "cache.h"
#include "renderer.h"
//#include "collider.h"
#include "shader.h"
#include "texture.h"
#include "mesh.h"
#include "material.h"
#include "input.h"
#include "light.h"

typedef struct obj_t {
    float pos[3];
    float rot[3];
    float angle;
    float scl[3];
} obj_t;

obj_t* make_obj(json_object* j_obj){
    obj_t* obj = malloc(sizeof(obj_t));

    json_object* j_current = json_object_object_get(j_obj, "pos");
    for (int i = 0; i < 3; i++){
        json_object* index = json_object_array_get_idx(j_current, i);
        obj->pos[i] = json_object_get_double(index);
    }
    j_current = json_object_object_get(j_obj, "rot");
    for (int i = 0; i < 3; i++){
        json_object* index = json_object_array_get_idx(j_current, i);
        obj->rot[i] = json_object_get_double(index);
    }
    json_object* index = json_object_array_get_idx(j_current, 3);
    obj->angle = json_object_get_double(index);
    j_current = json_object_object_get(j_obj, "scl");
    for (int i = 0; i < 3; i++){
        json_object* index = json_object_array_get_idx(j_current, i);
        obj->scl[i] = json_object_get_double(index);
    }

    return obj;
}

scene_t* scene_load(const char* path){
    scene_t* scene = malloc(sizeof(scene_t)); 
    scene->time = 0;
    scene->rend = NULL;
    scene->idcache = NULL;
    scene->proj = malloc(sizeof(mat4));
    scene->view = malloc(sizeof(mat4));
    glm_vec3_copy((vec3){0, 0, -1}, scene->camera_rot);
    glm_vec3_copy((vec3){0, 0, 0}, scene->camera_pos);

    FILE *fp;
	fp = fopen(path, "r");
    fseek(fp, 0, SEEK_END);
    unsigned int f_size = ftell(fp); 
    fseek(fp, 0, SEEK_SET);
    char* buf = malloc(f_size + 1);
	fread(buf, f_size, 1, fp);
	fclose(fp);

    json_object* j_scene = json_tokener_parse(buf);

    free(buf);

    int* texind = malloc(sizeof(int) * 2);
    texind[0] = 0;
    texind[1] = 1;

    json_object* j_lighting = json_object_object_get(j_scene, "lighting");
    unsigned int lpointc, lspotc;
    light_t* lighting = lighting_load(&scene->idcache, j_lighting, &lpointc, &lspotc);
    s_keyl_t* sinput = NULL;
    shader_input_push(&sinput, S_INT_T, 0, &lpointc);
    shader_input_push(&sinput, S_INT_T, 1, &lspotc);

    json_object* j_shaders = json_object_object_get(j_scene, "scene");
    unsigned int shader_count = json_object_array_length(j_shaders);
    rend_t** r_shader = (rend_t**)(&(scene->rend));
    for (int i = 0; i < shader_count; i++){
        json_object* j_shader = json_object_array_get_idx(j_shaders, i);
        unsigned int shader_id = shader_make_from_file(json_object_get_string(json_object_object_get(j_shader, "shader")), sinput);
        rend_shader_push(r_shader, shader_id);
        
        unsigned int unif_locs[] = {
            glGetUniformLocation(shader_id, "u_proj"),
            glGetUniformLocation(shader_id, "u_view"),
            glGetUniformLocation(shader_id, "u_model"),

            glGetUniformLocation(shader_id, "u_mat.diffuse"),
            glGetUniformLocation(shader_id, "u_mat.specular"),
            glGetUniformLocation(shader_id, "u_mat.shininess"),

            glGetUniformLocation(shader_id, "u_camera"),
        };

        lighting_bind(lighting, (void**)&(*r_shader)->uniforms, shader_id);

        unif_push(&(*r_shader)->uniforms, unif_locs[0], UNIF_MAT4, scene->proj);
        unif_push(&(*r_shader)->uniforms, unif_locs[1], UNIF_MAT4, scene->view);

        unif_push(&(*r_shader)->uniforms, unif_locs[3], UNIF_INT, texind + 0);
        unif_push(&(*r_shader)->uniforms, unif_locs[4], UNIF_INT, texind + 1);

        unif_push(&(*r_shader)->uniforms, unif_locs[6], UNIF_VEC3, scene->camera_pos);

        json_object* j_vaos = json_object_object_get(j_shader, "vaos");
        vao_r_t** r_vao = &(*r_shader)->vaos;
        unsigned int vao_count = json_object_array_length(j_vaos);
        for (int j = 0; j < vao_count; j++){
            json_object* j_vao = json_object_array_get_idx(j_vaos, j);

            mesh_t* mesh = mesh_make(json_object_get_string(json_object_object_get(j_vao, "vao")));
            rend_vao_push(*r_shader, mesh->vao, mesh->tcount);
            json_object* j_materials = json_object_object_get(j_vao, "materials");
            mat_r_t** r_material = &(*r_vao)->mats;
            unsigned int material_count = json_object_array_length(j_materials);
            for (int k = 0; k < material_count; k++){
                json_object* j_material = json_object_array_get_idx(j_materials, k);

                material_t* material = mat_make_from_file(&scene->idcache, json_object_get_string(json_object_object_get(j_material, "material")));
                rend_material_push(*r_vao, material);

                unif_push(&(*r_material)->uniforms, unif_locs[5], UNIF_FLOAT, &material->shininess);

                json_object* j_objects = json_object_object_get(j_material, "objects");
                obj_r_t** r_object = &(*r_material)->objs;
                unsigned int object_count = json_object_array_length(j_objects);
                for (int l = 0; l < object_count; l++){
                    json_object* j_object = json_object_array_get_idx(j_objects, l);

                    rend_object_push(*r_material);
                    obj_t* obj = make_obj(j_object);
                    mat4* obj_trans = malloc(sizeof(mat4));
                    glm_mat4_identity(*obj_trans);
                    glm_scale(*obj_trans, obj->scl);
                    glm_translate(*obj_trans, obj->pos);
                    glm_rotate(*obj_trans, obj->angle, obj->rot);
                    free(obj);
                    unif_push(&(*r_object)->uniforms, unif_locs[2], UNIF_MAT4, obj_trans);

                    json_object* j_obj_cache = json_object_object_get(j_object, "id");
                    if (j_obj_cache != NULL)
                        cache_push((cache_l**)(&scene->idcache), json_object_get_string(j_obj_cache), obj_trans);

                    r_object = &(*r_object)->next;
                }
                r_material = &(*r_material)->next;
            }
            r_vao = &(*r_vao)->next;
        }
        r_shader = &(*r_shader)->next;
    }
    json_object_put(j_scene);

    printf("[INFO] Scene \"%s\" successfully loaded\n", path);

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
    if (glm_vec3_angle((vec3){0, 1, 0}, rot) > 0.3f &&
        glm_vec3_angle((vec3){0, -1, 0}, rot) > 0.3f &&
        ((input_t*)input)->m.lock){
        glm_vec3_copy(rot, scene->camera_rot);
    }

    glm_vec3_normalize(mov);
    glm_vec3_scale(mov, -2 * delta, mov);
    glm_vec3_rotate_m4(trans, mov, mov);
    glm_vec3_add(scene->camera_pos, mov, scene->camera_pos);

    glm_look(scene->camera_pos, scene->camera_rot, (vec3){0, 1, 0}, scene->view);

    glm_perspective(glm_rad(90.0f), ((float)wwidth)/wheight, 0.1f, 100.0f, *(mat4*)scene->proj);

    lspot_t* spotl = cache_get(scene->idcache, "spot");
    glm_vec3_rotate(spotl->direction, sin(delta), (vec3){0,1,0});

    rend_draw(scene->rend);
}