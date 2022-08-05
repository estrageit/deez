#include "material.h"

#include "texture.h"
#include "cache.h"

#include <cglm/cglm.h>

#include <json-c/json.h>
#include <stdio.h>

cache_l* cache_mat = NULL;

material_t* mat_make_from_file(void* idcache, const char* path){
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
    free(buf);

    mat->diffuse = texture_load(json_object_get_string(json_object_object_get(j_mat, "diffuse")));
    mat->specular = texture_load(json_object_get_string(json_object_object_get(j_mat, "specular")));
    mat->shininess = json_object_get_double(json_object_object_get(j_mat, "shininess"));

    json_object* j_mat_cache = json_object_object_get(j_mat, "id");
    if (j_mat_cache != NULL)
        cache_push((cache_l**)idcache, json_object_get_string(j_mat_cache), mat);

    json_object_put(j_mat);

    cache_push(&cache_mat, path, mat);

    return mat;
}