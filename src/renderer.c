#include "renderer.h"

#include <stdlib.h>
#include <stdio.h>

#include <glad/gl.h>

rend_t *rend_make(unsigned int shader, unsigned int vao, unsigned int count, unif_v uniforms, uintv textures){
    rend_t *head = malloc(sizeof(rend_t));
    head->shader = shader;
    head->vao = vao;
    head->count = count;
    head->uniforms = uniforms;
    head->textures = textures;
    head->next = NULL;
    return head;
}
void rend_push(rend_t *head, unsigned int shader, unsigned int vao, unsigned int count, unif_v uniforms, uintv textures){
    rend_t *node = malloc(sizeof(rend_t));
    node->shader = shader;
    node->vao = vao;
    node->count = count;
    node->uniforms = uniforms;
    node->textures = textures;
    lnk_push((lnk*)head, (lnk*)node);
}
void rend_remove(rend_t *head, rend_t *node){
    lnk_remove((lnk*)head, (lnk*)node);
}
void rend_removeat(rend_t *head, unsigned int index){
    lnk_removeat((lnk*)head, index);
}
void rend_destroy(rend_t *head){
    lnk_destroy((lnk*)head);
}
void rend_draw(rend_t *head){
    rend_t *current = head;
    while(current != NULL){
        if(current->vao != 0 && current->shader != 0){
            glUseProgram(current->shader);
            for (int i = 0; i < current->uniforms.size; i++){
                switch (current->uniforms.data[i].type){
                    case UNIF_INT:
                        glUniform1i(current->uniforms.data[i].location, *(int*)current->uniforms.data[i].data);
                        break;
                    case UNIF_FLOAT:
                        glUniform1f(current->uniforms.data[i].location, *(float*)current->uniforms.data[i].data);
                        break;
                    case UNIF_VEC2:
                        glUniform2fv(current->uniforms.data[i].location, 1, (float*)current->uniforms.data[i].data);
                        break;
                    case UNIF_MAT4:
                        glUniformMatrix4fv(current->uniforms.data[i].location, 1, GL_FALSE, (float*)current->uniforms.data[i].data);
                        break;
                    case UNIF_VEC3:
                        glUniform3fv(current->uniforms.data[i].location, 1, (float*)current->uniforms.data[i].data);
                        break;
                    default:
                        printf("[ERROR] Uniform type (%d) not supported by renderer\n", current->uniforms.data[i].type);
                        break;
                }
            }
            glBindVertexArray(current->vao);
            glDrawElements(GL_TRIANGLES, current->count, GL_UNSIGNED_INT, 0);
            //printf("%d %d %d\n", glGetError(), current->shader, current->vao);
        }
        current = current->next;
    }
}