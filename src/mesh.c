#include "mesh.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <json-c/json.h>

#include <glad/gl.h>


//todo: add checks in this function for improperly formatted json files!!
unsigned int mesh_make(const char* path){
    FILE *fp;
	fp = fopen(path, "r");

    fseek(fp, 0, SEEK_END);
    unsigned int f_size = ftell(fp); 
    fseek(fp, 0, SEEK_SET);

    char* buf = malloc(f_size);

	fread(buf, f_size, 1, fp);
	fclose(fp);

    json_object* j_mesh;
    json_object* j_vertices;
    json_object* j_indices;
    j_mesh = json_tokener_parse(buf);

    json_object_object_get_ex(j_mesh, "vertices", &j_vertices);
    json_object_object_get_ex(j_mesh, "indices", &j_indices);

    unsigned int indexc = json_object_array_length(j_indices);
    unsigned int* indices = malloc(sizeof(unsigned int) * indexc);
    for (int i = 0; i < indexc; i++){
        json_object* index = json_object_array_get_idx(j_indices, i);
        indices[i] = json_object_get_int(index);
    }
    
    unsigned int vatc = json_object_array_length(j_vertices);
    json_object** j_vat = malloc(sizeof(json_object*) * vatc);

    for (int i = 0; i < vatc; i++){
        j_vat[i] = json_object_array_get_idx(j_vertices, i);
    }

    unsigned int vertexc = json_object_array_length(j_vat[0]);
    
    free(j_vat);
    json_object_put(j_mesh);
    free(buf);
    free(indices);
}

unsigned int mesh_make_vao(mesh_t *mesh){
    unsigned int vao, vbo, ibo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh->vsize, mesh->vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->isize, mesh->indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return vao;
}