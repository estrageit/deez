#include "link.h"

#include <stdlib.h>
#include <stdio.h>

unsigned int lnk_count(lnk *head){
    unsigned int count = 0;
    lnk *current = head;
    while(current != NULL){
        count++;
        current = current->next;
    }
    return count;
}
void lnk_push(lnk *head, lnk *node){
    lnk *current = head;
    while(current->next != NULL){
        current = current->next;
    }
    current->next = node;
    node->next = NULL;
}
void lnk_remove(lnk *head, lnk *node){
    lnk *current = head;
    while(current->next != node){
        current = current->next;
    }
    current->next = node->next;
    free(node);
}
void lnk_removeat(lnk *head, unsigned int index){
    lnk *current = head;
    unsigned int i = 0;
    while(i < index){
        current = current->next;
        i++;
    }
    current->next = current->next->next;
    free(current->next);
}
void lnk_destroy(lnk *head){
    lnk *current = head;
    while(current != NULL){
        lnk *temp = current;
        current = current->next;
        free(temp);
    }
}