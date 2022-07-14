/**
 * @file ll.h
 * @author Bhanu Prakash
 * @brief 
 * @version 0.1
 * @date 2021-12-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef _LL_H_
#define _LL_H_

typedef struct node {
    void *data;
    struct node *next;
}node_t;

typedef struct list
{
    node_t *head;
}list_t;

list_t* init_ll();
void deinit_ll(list_t* list);
void append_ll(list_t* list, void * data);
void del_ll(list_t* list, node_t* node);

//void display_ll(list_t* list); // cannot be implemented here since the data in structure is void and depends on application

// init, deinit, add/append, del

#endif  // _LL_H