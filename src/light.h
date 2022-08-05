#pragma once

#define LT_DIR 0
#define LT_POINT 1
#define LT_SPOT 2

typedef struct light_t{
    struct light_t* next;
    unsigned int type;
} light_t;

typedef struct ldir_t{
    light_t* next;
    unsigned int type;

    float direction[3];
  
    float ambient[3];
    float diffuse[3];
    float specular[3];
} ldir_t;

typedef struct lpoint_t{
    light_t* next;
    unsigned int type;

    float position[3];
    
    float constant;
    float linear;
    float quadratic;

    float ambient[3];
    float diffuse[3];
    float specular[3];
} lpoint_t;

typedef struct lspot_t{
    light_t* next;
    unsigned int type;

    float position[3];

    float direction[3];
    float cutoff;
    float outercutoff;

    float constant;
    float linear;
    float quadratic;

    float ambient[3];
    float diffuse[3];
    float specular[3];
} lspot_t;

light_t* lighting_load(void* idcache, void* j_lighting, unsigned int* pointc, unsigned int* spotc);
void lighting_bind(light_t* lights, void** unif, unsigned int s);