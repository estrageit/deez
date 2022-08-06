#include "light.h"

#include <json-c/json.h>
#include <string.h>
#include <glad/gl.h>
#include <stdio.h>

#include "link.h"
#include "renderer.h"
#include "cache.h"

light_t* lighting_load(void* idcache, void* j_lighting, unsigned int* pointc, unsigned int* spotc){
    light_t* res = NULL;
    *pointc = 0;
    *spotc = 0;

    unsigned int light_count = json_object_array_length(j_lighting);
    for (int i = 0; i < light_count; i++){
        json_object* j_light = json_object_array_get_idx(j_lighting, i);

        const char* ltype = json_object_get_string(json_object_object_get(j_light, "type"));
        light_t* curl = NULL;
        if (strcmp(ltype, "dir") == 0){
            curl = malloc(sizeof(ldir_t));
            curl->type = LT_DIR;
            curl->next = NULL;
            json_object* j_cur = json_object_object_get(j_light, "direction");
            for (int i = 0; i < 3; i++)
                ((ldir_t*)curl)->direction[i] = json_object_get_double(json_object_array_get_idx(j_cur, i));

            j_cur = json_object_object_get(j_light, "ambient");
            for (int i = 0; i < 3; i++)
                ((ldir_t*)curl)->ambient[i] = json_object_get_double(json_object_array_get_idx(j_cur, i));
            j_cur = json_object_object_get(j_light, "diffuse");
            for (int i = 0; i < 3; i++)
                ((ldir_t*)curl)->diffuse[i] = json_object_get_double(json_object_array_get_idx(j_cur, i));
            j_cur = json_object_object_get(j_light, "specular");
            for (int i = 0; i < 3; i++)
                ((ldir_t*)curl)->specular[i] = json_object_get_double(json_object_array_get_idx(j_cur, i));
        } else if (strcmp(ltype, "point") == 0){
            curl = malloc(sizeof(lpoint_t));
            curl->type = LT_POINT;
            curl->next = NULL;
            json_object* j_cur = json_object_object_get(j_light, "position");
            for (int i = 0; i < 3; i++)
                ((lpoint_t*)curl)->position[i] = json_object_get_double(json_object_array_get_idx(j_cur, i));

            ((lpoint_t*)curl)->constant =   json_object_get_double(json_object_object_get(j_light, "constant"));
            ((lpoint_t*)curl)->linear =     json_object_get_double(json_object_object_get(j_light, "linear"));
            ((lpoint_t*)curl)->quadratic =  json_object_get_double(json_object_object_get(j_light, "quadratic"));

            j_cur = json_object_object_get(j_light, "ambient");
            for (int i = 0; i < 3; i++)
                ((lpoint_t*)curl)->ambient[i] = json_object_get_double(json_object_array_get_idx(j_cur, i));
            j_cur = json_object_object_get(j_light, "diffuse");
            for (int i = 0; i < 3; i++)
                ((lpoint_t*)curl)->diffuse[i] = json_object_get_double(json_object_array_get_idx(j_cur, i));
            j_cur = json_object_object_get(j_light, "specular");
            for (int i = 0; i < 3; i++)
                ((lpoint_t*)curl)->specular[i] = json_object_get_double(json_object_array_get_idx(j_cur, i));
            (*pointc)++;
        } else if (strcmp(ltype, "spot") == 0){
            curl = malloc(sizeof(lspot_t));
            curl->type = LT_SPOT;
            curl->next = NULL;
            json_object* j_cur = json_object_object_get(j_light, "position");
            for (int i = 0; i < 3; i++)
                ((lspot_t*)curl)->position[i] = json_object_get_double(json_object_array_get_idx(j_cur, i));

            j_cur = json_object_object_get(j_light, "direction");
            for (int i = 0; i < 3; i++)
                ((lspot_t*)curl)->direction[i] = json_object_get_double(json_object_array_get_idx(j_cur, i));
            ((lspot_t*)curl)->cutoff =      json_object_get_double(json_object_object_get(j_light, "cutoff"));
            ((lspot_t*)curl)->outercutoff = json_object_get_double(json_object_object_get(j_light, "outercutoff"));

            ((lspot_t*)curl)->constant =   json_object_get_double(json_object_object_get(j_light, "constant"));
            ((lspot_t*)curl)->linear =     json_object_get_double(json_object_object_get(j_light, "linear"));
            ((lspot_t*)curl)->quadratic =  json_object_get_double(json_object_object_get(j_light, "quadratic"));

            j_cur = json_object_object_get(j_light, "ambient");
            for (int i = 0; i < 3; i++)
                ((lspot_t*)curl)->ambient[i] = json_object_get_double(json_object_array_get_idx(j_cur, i));
            j_cur = json_object_object_get(j_light, "diffuse");
            for (int i = 0; i < 3; i++)
                ((lspot_t*)curl)->diffuse[i] = json_object_get_double(json_object_array_get_idx(j_cur, i));
            j_cur = json_object_object_get(j_light, "specular");
            for (int i = 0; i < 3; i++)
                ((lspot_t*)curl)->specular[i] = json_object_get_double(json_object_array_get_idx(j_cur, i));
            (*spotc)++;
        } else {
            printf("[ERROR] Light type \"%s\" does not exist\n", ltype);
        }
        json_object* j_light_cache = json_object_object_get(j_light, "IDs");
        if (j_light_cache != NULL){
            unsigned int idc = json_object_array_length(j_light_cache);
            for (int i = 0; i < idc; i++){
                cache_push((cache_l**)idcache, json_object_get_string(json_object_array_get_idx(j_light_cache, i)), curl);
            }
        }
        lnk_push((lnk**)(&res), (lnk*)curl);
    }
    return res;
}

unsigned int ul(const char* l, unsigned int s, unsigned int i){
    char b[128];
    sprintf(b, l, i);
    if (glGetUniformLocation(s, b) == -1)
    {
        printf("%s\n", b);
    }
    
    return glGetUniformLocation(s, b);
}

void lighting_bind(light_t* lights, void** unif, unsigned int s){
    light_t* curl = lights;
    unsigned int pointc = 0;
    unsigned int spotc = 0;
    while (curl != NULL){
        switch (curl->type){
        case LT_DIR:
            unif_push((unif_t**)unif, glGetUniformLocation(s, "u_ldir.direction"), UNIF_VEC3, ((ldir_t*)curl)->direction);
            unif_push((unif_t**)unif, glGetUniformLocation(s, "u_ldir.ambient"), UNIF_VEC3, ((ldir_t*)curl)->ambient);
            unif_push((unif_t**)unif, glGetUniformLocation(s, "u_ldir.diffuse"), UNIF_VEC3, ((ldir_t*)curl)->diffuse);
            unif_push((unif_t**)unif, glGetUniformLocation(s, "u_ldir.specular"), UNIF_VEC3, ((ldir_t*)curl)->specular);
            break;
        case LT_POINT:
            unif_push((unif_t**)unif, ul("u_lpoint[%d].position", s, pointc), UNIF_VEC3, ((lpoint_t*)curl)->position);

            unif_push((unif_t**)unif, ul("u_lpoint[%d].constant", s, pointc), UNIF_FLOAT, &((lpoint_t*)curl)->constant);
            unif_push((unif_t**)unif, ul("u_lpoint[%d].linear", s, pointc), UNIF_FLOAT, &((lpoint_t*)curl)->linear);
            unif_push((unif_t**)unif, ul("u_lpoint[%d].quadratic", s, pointc), UNIF_FLOAT, &((lpoint_t*)curl)->quadratic);

            unif_push((unif_t**)unif, ul("u_lpoint[%d].ambient", s, pointc), UNIF_VEC3, ((lpoint_t*)curl)->ambient);
            unif_push((unif_t**)unif, ul("u_lpoint[%d].diffuse", s, pointc), UNIF_VEC3, ((lpoint_t*)curl)->diffuse);
            unif_push((unif_t**)unif, ul("u_lpoint[%d].specular", s, pointc), UNIF_VEC3, ((lpoint_t*)curl)->specular);
            pointc++;
            break;
        case LT_SPOT:
            unif_push((unif_t**)unif, ul("u_lspot[%d].position", s, spotc), UNIF_VEC3, ((lspot_t*)curl)->position);

            unif_push((unif_t**)unif, ul("u_lspot[%d].direction", s, spotc), UNIF_VEC3, ((lspot_t*)curl)->direction);
            unif_push((unif_t**)unif, ul("u_lspot[%d].cutoff", s, spotc), UNIF_FLOAT, &((lspot_t*)curl)->cutoff);
            unif_push((unif_t**)unif, ul("u_lspot[%d].outercutoff", s, spotc), UNIF_FLOAT, &((lspot_t*)curl)->outercutoff);

            unif_push((unif_t**)unif, ul("u_lspot[%d].constant", s, spotc), UNIF_FLOAT, &((lspot_t*)curl)->constant);
            unif_push((unif_t**)unif, ul("u_lspot[%d].linear", s, spotc), UNIF_FLOAT, &((lspot_t*)curl)->linear);
            unif_push((unif_t**)unif, ul("u_lspot[%d].quadratic", s, spotc), UNIF_FLOAT, &((lspot_t*)curl)->quadratic);

            unif_push((unif_t**)unif, ul("u_lspot[%d].ambient", s, spotc), UNIF_VEC3, ((lspot_t*)curl)->ambient);
            unif_push((unif_t**)unif, ul("u_lspot[%d].diffuse", s, spotc), UNIF_VEC3, ((lspot_t*)curl)->diffuse);
            unif_push((unif_t**)unif, ul("u_lspot[%d].specular", s, spotc), UNIF_VEC3, ((lspot_t*)curl)->specular);
            spotc++;
            break;
        default:
            printf("[ERROR] Light type \"%d\" does not exist\n", curl->type);
            break;
        }
        curl = curl->next;
    }
}