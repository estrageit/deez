#pragma once

typedef struct scene_t {
    void* rend;
    void* coll;
    void* obj;
    int objc;
    float time;
    void* camera;
    void* proj;
} scene_t;

scene_t scene_load(const char* path);
void scene_update(scene_t* scene, float time, int wwidth, int wheight);