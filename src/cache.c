#include "cache.h"

#include <string.h>
#include <stdlib.h>

#include "link.h"

void* cache_get(cache_l* cache, const char* path){
    cache_l* cur = cache;
    while (cur != NULL){
        if (!strcmp(path, cur->path)){
            return cur->data;
        }
        cur = cur->next;
    }
    return NULL;
}

void cache_push(cache_l** cache, const char* path, void* data){
    cache_l* node = malloc(sizeof(cache_l));
    char* bpath = malloc(sizeof(char) * strlen(path) + 1);
    strcpy(bpath, path);
    node->data = data;
    node->path = bpath;
    node->next = NULL;
    lnk_push((lnk**)cache, (lnk*)node);
}