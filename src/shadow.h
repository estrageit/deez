#pragma once

#define SDW_T_SMPL 0 
#define SDW_T_OMNI 1
#define SDW_T_CSM  2

typedef struct shadow_t{
    struct shadow_t* next;
    unsigned int type;
} shadow_t;

typedef struct ssmpl_t{
    shadow_t* next;
    unsigned int type;
    void* light;
    unsigned int fbo;
} ssmpl_t;

typedef struct somni_t{
    shadow_t* next;
    unsigned int type;
    void* light;
} somni_t;

typedef struct scsm_t{
    shadow_t* next;
    unsigned int type;
    void* light;
} scsm_t;