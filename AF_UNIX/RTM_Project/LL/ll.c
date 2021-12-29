/**
 * @file ll.c
 * @author Bhanu Prakash
 * @brief single linked list implementation
 * @version 0.1
 * @date 2021-12-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "ll.h"

list_t* init_ll()
{
    list_t* list = (list_t*)malloc(sizeof(list_t));
    list->head = NULL;
    return list;
}

void append_ll(list_t* list, void * data)
{
    node_t *cur = list->head;

    //prepare new node with received data
    node_t *temp = (node_t *)malloc(sizeof(node_t));
    temp->data = data;
    temp->next = NULL;
    if(cur == NULL)
    {
        printf("adding first element to list\n");
        list->head = temp;
    }
    else
    {
        printf("adding element to list\n");
        while(cur->next != NULL)
        {
            cur =cur->next;

        }
        cur->next = temp;
    }
}

void del_ll(list_t* list, node_t* node)
{
    node_t* prev = list->head; 
    node_t* cur = list->head;
    while(cur != node)
    {
        prev = cur;
        cur = cur->next;
    }
    prev->next = cur->next;
    if(cur == list->head)
    {
        list->head = cur->next;
    }
    free(cur);
}

