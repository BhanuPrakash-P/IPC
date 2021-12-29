/**
 * @file main.c
 * @author Bhanu Prakash
 * @brief sample program to test linked list functionality
 * @version 0.1
 * @date 2021-12-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ll.h"

void display_ll(list_t* list)
{
    node_t* cur = list->head;
    printf("****LIST****\n");
    while(cur != NULL)
    {
        printf("%p: %d\n", cur, *(int*)cur->data);
        cur = cur->next;
    }
    printf("****END****");
}

node_t* find(list_t *list, int data)
{
    node_t*cur = list->head;
    while(cur != NULL)
    {
        if(*(int*)cur->data == data)
        {
            return cur;
        }
        cur = cur->next;
    }
    return cur;

}
int main()
{
    list_t *ll = init_ll();
    int i=0;
    int *arr;
    for (i = 0; i < 5; i++) {
        int *arr = malloc(sizeof(int));
        *arr = i;
        append_ll(ll, arr);
    }
    display_ll(ll);

#if 0
    node_t* node = find(ll, 3);
    printf("find result:%p: %d", node, *(int*)node->data);
#endif

    del_ll(ll, find(ll, 4));
    display_ll(ll);

}