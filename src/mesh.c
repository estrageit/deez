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
    
    unsigned int vert_s = 0;
    unsigned int *vats = malloc(sizeof(unsigned int) * vatc);
    for (int i = 0; i < vatc; i++){
        json_object* j_cvat = json_object_array_get_idx(j_vat[i], 0);
        vats[i] = json_object_array_length(j_cvat);
        vert_s += vats[i];
    }

    unsigned int vb_s = sizeof(float) * vertexc * vert_s;
    float* vertices = malloc(vb_s);

    printf("[INFO] Loading model \"%s\":\n\tVertex Count: %d\n\tAttribute count: %d\n\tVertex size: %d\n", path, vertexc, vatc, vert_s);

    for(int i = 0; i < vertexc; i++){
        float* cvert = vertices + i * vert_s;
        for(int j = 0; j < vatc; j++){
            json_object* j_cvat = json_object_array_get_idx(j_vat[j], i);
            unsigned int ats = json_object_array_length(j_cvat);
            for (int k = 0; k < ats; k++){
                json_object* cval = json_object_array_get_idx(j_cvat, k);
                cvert[k] = json_object_get_double(cval);
            }
            cvert += ats;
        }
    }
    json_object_put(j_mesh);

    unsigned int vao, vbo, ibo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vb_s, vertices, GL_STATIC_DRAW);

    for(int i = 0; i < vatc; i++){
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, vats[i], GL_FLOAT, GL_FALSE, vert_s * sizeof(float), (void*)(vats[i] * sizeof(float)));
    }

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexc, indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    free(j_vat);
    free(vats);
    free(buf);
    free(indices);
    free(vertices);

    return vao;
}