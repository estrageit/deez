#include "object.h"

#include "texture.h"
#include "cache.h"

#include <cglm/cglm.h>

#include <json-c/json.h>
#include <stdio.h>

cache_l* cache_mat = NULL;

material_t* mat_make_from_file(const char* path){
    material_t* cached = cache_get(cache_mat, path);
    if(cached){
        return cached;
    }

    printf("[INFO] Loading material \"%s\"\n", path);
    material_t* mat = malloc(sizeof(material_t));

    FILE *fp;
	fp = fopen(path, "r");
    fseek(fp, 0, SEEK_END);
    unsigned int f_size = ftell(fp); 
    fseek(fp, 0, SEEK_SET);
    char* buf = malloc(f_size + 1);
	fread(buf, f_size, 1, fp);
	fclose(fp);

    json_object* j_mat = json_tokener_parse(buf);

    json_object* j_current = json_object_object_get(j_mat, "specular");
    for (int i = 0; i < 3; i++){
        json_object* index = json_object_array_get_idx(j_current, i);
        mat->specular[i] = json_object_get_int(index);
    }

    j_current = json_object_object_get(j_mat, "diffuse");
    for (int i = 0; i < 3; i++){
        json_object* index = json_object_array_get_idx(j_current, i);
        mat->diffuse[i] = json_object_get_int(index);
    }

    j_current = json_object_object_get(j_mat, "ambient");
    for (int i = 0; i < 3; i++){
        json_object* index = json_object_array_get_idx(j_current, i);
        mat->ambient[i] = json_object_get_int(index);
    }

    j_current = json_object_object_get(j_mat, "shininess");
    mat->shininess = (float)(json_object_get_double(j_current));

    j_current = json_object_object_get(j_mat, "texture");
    mat->texture = 0;
    const char* tex_path = json_object_get_string(j_current);
    if (*tex_path != '\0')
        mat->texture = texture_load(tex_path);

    //free(buf);
    //json_object_put(j_mat);

    cache_push(&cache_mat, path, mat);

    return mat;
}

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