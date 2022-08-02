#include "renderer.h"

#include <stdlib.h>
#include <stdio.h>

#include <glad/gl.h>

#include "link.h"

void unif_set(unif_t* head){
    unif_t *current = head;
    while (current != NULL){
        switch (current->type){
            case UNIF_INT:
                glUniform1i(current->location, *(int*)current->data);
                break;
            case UNIF_FLOAT:
                glUniform1f(current->location, *(float*)current->data);
                break;
            case UNIF_VEC2:
                glUniform2fv(current->location, 1, (float*)current->data);
                break;
            case UNIF_MAT4:
                glUniformMatrix4fv(current->location, 1, GL_FALSE, (float*)current->data);
                break;
            case UNIF_VEC3:
                glUniform3fv(current->location, 1, (float*)current->data);
                break;
            default:
                printf("[ERROR] Uniform type (%d) not supported by renderer\n", current->type);
                break;
        }
        current = current->next;
    }
}

void unif_push(unif_t** head, int location, int type, void* data){
    if (location == -1)
        return;
    unif_t *node = malloc(sizeof(unif_t));
    node->next = NULL;
    node->location = location;
    node->type = type;
    node->data = data;
    lnk_push((lnk**)head, (lnk*)node);
}

void rend_shader_push(rend_t** head, unsigned int shader){
    rend_t* node = malloc(sizeof(rend_t));
    node->next = NULL;
    node->shader = shader;
    node->vaos = NULL;
    node->uniforms = NULL;
    lnk_push((lnk**)head, (lnk*)node);
}

void rend_vao_push(rend_t* renderer_shader, unsigned int vao, unsigned int count){
    vao_r_t* node = malloc(sizeof(vao_r_t));
    node->next = NULL;
    node->count = count;
    node->vao = vao;
    node->mats = NULL;
    node->uniforms = NULL;
    lnk_push((lnk**)&renderer_shader->vaos, (lnk*)node);
}

void rend_material_push(vao_r_t* renderer_vao, unsigned int texture){
    mat_r_t* node = malloc(sizeof(mat_r_t));
    node->next = NULL;
    node->texture = texture;
    node->objs = NULL;
    node->uniforms = NULL;
    lnk_push((lnk**)&renderer_vao->mats, (lnk*)node);
}

void rend_object_push(mat_r_t* renderer_material){
    obj_r_t* node = malloc(sizeof(obj_r_t));
    node->next = NULL;
    node->uniforms = NULL;
    lnk_push((lnk**)&renderer_material->objs, (lnk*)node);
}

void rend_draw(rend_t* head){ 
    rend_t *cur_sh = head;
    while (cur_sh != NULL){
        glUseProgram(cur_sh->shader);
        unif_set(cur_sh->uniforms);
        vao_r_t *cur_vao = cur_sh->vaos;
        while (cur_vao != NULL){
            glBindVertexArray(cur_vao->vao);
            unif_set(cur_vao->uniforms);
            mat_r_t *cur_mat = cur_vao->mats;
            while (cur_mat != NULL){
                glBindTexture(GL_TEXTURE0, cur_mat->texture);
                unif_set(cur_mat->uniforms);
                obj_r_t *cur_obj = cur_mat->objs;
                while (cur_obj != NULL){
                    unif_set(cur_obj->uniforms);
                    glDrawElements(
                        GL_TRIANGLES,
                        cur_vao->count,
                        GL_UNSIGNED_INT,
                        0);
                    cur_obj = cur_obj->next;
                }
                cur_mat = cur_mat->next;
            }
            cur_vao = cur_vao->next;
        }
        cur_sh = cur_sh->next;
    }
}