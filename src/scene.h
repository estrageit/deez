#pragma once

typedef struct camera_t {
    float pos[3];
    float rot[3];
    void* trans;
} camera_t;

void camera_transform(camera_t* cam);

typedef struct scene_t {
    void* rend;
    void* coll;
    void* obj;
    float light_pos[3];
    void* light_trans;
    int objc;
    float time;
    void* proj;
    camera_t cam;
} scene_t;

scene_t* scene_load(const char* path);
void scene_update(scene_t* scene, float time, int wwidth, int wheight, void* input);