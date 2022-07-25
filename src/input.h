#pragma once
#include <stdint.h>

typedef struct keys_t{
    uint64_t down;
    uint64_t up;
    uint64_t k;
} keys_t;

#define I_MOUSE_LEFT 0
#define I_MOUSE_RIGHT 1
#define I_MOUSE_MIDDLE 2

typedef struct mouse_t{
    uint8_t m;
    uint8_t down;
    uint8_t up;
    float x, y;
    char lock;
} mouse_t;

typedef struct input_t{
    keys_t k;
    mouse_t m;
} input_t;

void input_update(void* window, input_t* input);
void input_lock_mouse(void* window, input_t* input, char lock);