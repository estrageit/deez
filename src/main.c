#include <stdio.h>
#include <stdlib.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "scene.h"

#include "shader.h"
#include "input.h"

int wwidth = 640, wheight = 480;

void framebuffer_size_callback(GLFWwindow *window, int width, int height){
    glViewport(0, 0, width, height);
    wwidth = width;
    wheight = height;
}

int main(void){
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(wwidth, wheight, "Deez", NULL, NULL);
    if (!window){
        glfwTerminate();
        return -1;
    }    

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glEnable(GL_DEPTH_TEST);  

    printf("[INFO] OpenGL Version: %s\n", glGetString(GL_VERSION));

    glfwSetCursorPos(window, wwidth / 2.0, wheight / 2.0);

    scene_t* scene = scene_load("deez");
    input_t input;
    for(int i = 0; i < sizeof(input_t); i++){
        ((char*)(&input))[i] = 0;
    }
    input_lock_mouse(window, &input, 1);

    char lock = 1;

    while (!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        input_update(window, &input);
        if (input.k.down & (uint64_t)1 << ('E' - 32)){
            lock ^= 1;
            input_lock_mouse(window, &input, lock);
        }
        scene_update(scene, glfwGetTime(), wwidth, wheight, &input);
        
        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
