#pragma once

typedef struct scene_t {
    float time;
    void* rend;
    void* idcache;

    void* proj;
    void* view;
    float camera_pos[3];
    float camera_rot[3];
    float light_pos[3];
} scene_t;

scene_t* scene_load(const char* path);
void scene_update(scene_t* scene, float time, int wwidth, int wheight, void* input);