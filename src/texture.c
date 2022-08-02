#include "texture.h"

#include <stdio.h>
#include <stdlib.h>

#include <glad/gl.h>
#include <stbi/stb_image.h>

#include "cache.h"

cache_l* cache_t = NULL;

unsigned int texture_load(const char* path){
    unsigned int cached = (uint64_t)cache_get(cache_t, path);
    if(cached){
        return cached;
    }

    printf("[INFO] Loading texture \"%s\"\n", path);
    unsigned int texture;  
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if(data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        printf("[ERROR] Failed to load texture\n");
        return 0;
    }
    stbi_image_free(data);

    cache_push(&cache_t, path, (void*)(uint64_t)texture);
    return texture;
}