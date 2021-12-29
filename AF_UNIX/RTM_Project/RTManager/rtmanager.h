/**
 * @file rtmanager.h
 * @author Bhanu Prakash
 * @brief Routing table manager
 * @version 0.1
 * @date 2021-12-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef _RT_MANAGER_
#define _RT_MANAGER_

#define IP_LEN 16
#define IF_LEN 32

typedef struct list list_t;
typedef struct node node_t;

typedef struct RTEntry{
    char destination[16];
    int mask;
    char gway_ip[16];
    char oif[32];
}RTEntry_t;

//supported operations: Create, Update, Delete, Display
node_t* find_routing_entry(list_t* routing_table, char* destination, int mask);
void update_routing_entry(node_t* routing_node, char* gateway, char* oif);
void display_routing_table(list_t* routing_table);


#endif //_RT_MANAGER_