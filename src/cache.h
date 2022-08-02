#pragma once

typedef struct cache_l{
    struct cache_l* next;
    const char* path;
    void* data;
} cache_l;

void* cache_get(cache_l* cache, const char* path);
void cache_push(cache_l** cache, const char* path, void* data);