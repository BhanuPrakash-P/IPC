/**
 * @file rtmanager.c
 * @author Bhanu Prakash
 * @brief Routing table manager
 * @version 0.1
 * @date 2021-12-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rtmanager.h"
#include "../LL/ll.h"

node_t* find_routing_entry(list_t* routing_table, char* destination, int mask)
{
    node_t* node = routing_table->head;
    while(node != NULL)
    {
        RTEntry_t routing_entry = *((RTEntry_t *)node->data);
        if( (strcmp(routing_entry.destination, destination) == 0) && 
            (routing_entry.mask == mask))
            {
                //printf("Entry found in routing table\n");
                return node;
            }
        node = node->next;
    }
    return NULL;

}

void update_routing_entry(node_t* routing_node, char* gateway, char* oif)
{
    RTEntry_t* routing_entry = routing_node->data;
    memset(routing_entry->gway_ip, 0, IP_LEN);
    memset(routing_entry->oif, 0, IF_LEN);
    memcpy(routing_entry->gway_ip, gateway, strlen(gateway));
    memcpy(routing_entry->oif, oif, strlen(oif));

}

void display_routing_table(list_t* routing_table)
{
    node_t* node = routing_table->head;
    int print = 1;
    while(node != NULL)
    {
        if(print)
        {
            printf("\n***********Rounting Table*************\n");
            printf("destination\tMask\tGateway\t\toif\t\n");
            print = 0;
        }
        RTEntry_t routing_entry = *((RTEntry_t *)node->data);
        printf("%s | %d\t | %s | %s\n", routing_entry.destination,routing_entry.mask, routing_entry.gway_ip, routing_entry.oif);
        node = node->next;
    }

}