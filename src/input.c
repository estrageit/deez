#include "input.h"

#include <GLFW/glfw3.h>
#include <stdio.h>

void input_update(void* window, input_t* input){
    input->k.down = 0;
    for (int i = 0; i < 64; i++){
        input->k.down |= (~input->k.k) & ((((uint64_t)1) * glfwGetKey((GLFWwindow*)window, 32 + i)) << i);
    }
    input->k.k = 0;
    for (int i = 0; i < 64; i++){
        input->k.k |= (((uint64_t)1) * glfwGetKey((GLFWwindow*)window, 32 + i)) << i;
    }
    int ww, wh;
    glfwGetWindowSize((GLFWwindow*)window, &ww, &wh);
    double cx, cy;
    glfwGetCursorPos((GLFWwindow*)window, &cx, &cy);
    input->m.x = ((cx / ww) * 2.0f) - 1.0f;
    input->m.y = ((cy / wh) * 2.0f) - 1.0f;
    if(input->m.lock)
        glfwSetCursorPos((GLFWwindow*)window, ww / 2.0, wh / 2.0);
}
void input_lock_mouse(void* window, input_t* input, char lock){
    if (lock)
        glfwSetInputMode((GLFWwindow*)window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    else
        glfwSetInputMode((GLFWwindow*)window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    input->m.lock = lock;
}