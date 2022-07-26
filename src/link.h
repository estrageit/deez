#pragma once

typedef struct lnk {
    struct lnk *next;
} lnk;

unsigned int lnk_count(lnk *head);
void lnk_push(lnk **head, lnk *node);
void lnk_remove(lnk *head, lnk *node);
void lnk_removeat(lnk *head, unsigned int index);
void lnk_destroy(lnk *head);