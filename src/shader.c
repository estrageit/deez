#include "shader.h"

#include <glad/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cache.h"

unsigned int compile_shader(int type, const char* source){
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char log[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(shader, 1024, NULL, log);
        printf("[ERROR] COMPILING SHADER OF TYPE %s: %s\n", type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT", log);
        return 0;
    }

    return shader;
}

unsigned int shader_make(const char* vertex, const char* fragment){
    unsigned int vert_id = compile_shader(GL_VERTEX_SHADER, vertex);
    unsigned int frag_id = compile_shader(GL_FRAGMENT_SHADER, fragment);

    unsigned int id = glCreateProgram();
    glAttachShader(id, vert_id);
    glAttachShader(id, frag_id);
    glLinkProgram(id);

    int success;
    char log[1024];

    glGetProgramiv(id, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(id, 1024, NULL, log);
        printf("[ERROR] LINKING SHADER: %s\n", log);
        id = 0;
    }

    glDeleteShader(vert_id);
    glDeleteShader(frag_id);

    return id;
}

cache_l* cache_s = NULL;

unsigned int shader_make_from_file(const char* path){
    unsigned int cached = (uint64_t)cache_get(cache_s, path);
    if(cached){
        return cached;
    }

    printf("[INFO] Loading shader \"%s\"\n", path);
    FILE *fp;
    unsigned int size;
    char *buf;

    fp = fopen(path, "rb");
    if (fp == NULL) {
        printf("[ERROR] Can't open shader file %s\n", path);
        return 0;
    }

    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    buf = (char*)malloc(size + 1);

    if (fread(buf, 1, size, fp) != size) {
        fclose(fp);
        free(buf);
        printf("[ERROR] Could open shader file '%s' but reading was not possible", path);
        return 0;
    }

    fclose(fp);

    char* src_vert;
    char* src_frag;

    buf[size] = '\0';

    for(int i = 0; i < size; i++){
        if(buf[i] != '$')
            continue;
        char* key;
        unsigned int keylen = 0;
        while(1){
            i++;
            if(buf[i] == ' '){
                key = buf + i - keylen;
                buf[i] = '\0';
                break;
            }
            else if(buf[i] == '\0'){
                printf("[ERROR] shader '%s' formatted incorrectly: key doesn't end with a space\n", path);
            }
            keylen++;
        }
        if(strcmp(key, "SHADER_TYPE") == 0){
            (key - 1)[0] = '\0';
            char* arg = key + keylen + 1;
            char tempchar = arg[4];
            arg[4] = '\0';
            if (strcmp(arg, "VERT") == 0)
            {
                src_vert = arg + 4;
            }
            else if (strcmp(arg, "FRAG") == 0)
            {
                src_frag = arg + 4;
            }
            else
                printf("[ERROR] shader '%s' formatted incorrectly: SHADER_TYPE '%s' is incorrect\n", path, arg);
            arg[4] = tempchar;
        }
        else if(strcmp(key, "INT") == 0){
        
        }
        else{
            printf("[ERROR] shader '%s' formatted incorrectly: incorrect key '%s'\n", path, key);
        }
    }

    //printf("SRC_VERT : %s\n", src_vert);
    //printf("SRC_FRAG : %s\n", src_frag);

    unsigned int shader_id = shader_make(src_vert, src_frag);
    free(buf);

    cache_push(&cache_s, path, (void*)(uint64_t)shader_id);

    return shader_id;
}