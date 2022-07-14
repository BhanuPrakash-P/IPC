/**
 * @file syncmsg.c
 * @author Bhanu Prakash
 * @brief sync message processing helper
 * @version 0.1
 * @date 2021-12-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "syncmsg.h"
#include "../LL/ll.h"
#include "../RTManager/rtmanager.h"

void process_sync_message(list_t* routing_table, sync_msg_t* sync_msg)
{
    node_t* node = NULL;
    //Check whether the entry exists in the routing table already
    node = find_routing_entry(routing_table, sync_msg->routing_entry.destination, sync_msg->routing_entry.mask);
    switch(sync_msg->opcode)
    {
        case CREATE:
            if(!node)
            { 
                append_ll(routing_table, &sync_msg->routing_entry);
                node = find_routing_entry(routing_table, sync_msg->routing_entry.destination, sync_msg->routing_entry.mask);
                if(node)
                {
                    RTEntry_t *routing_entry = (RTEntry_t *)node->data;
                    printf("New Entry added to Routing Table: IP Addr: %s/%d, gateway:%s, OIF:%s\n", routing_entry->destination, routing_entry->mask,
                                routing_entry->gway_ip, routing_entry->oif);
                }
                else
                {
                    printf("Failed to add new entry into routing table\n");
                }
            }
            else
            {
                printf("Entry for %s/%d is already available\n", sync_msg->routing_entry.destination, sync_msg->routing_entry.mask);
            }
            break;

        case DELETE:
            if(node)
            {
                del_ll(routing_table, node);
                node = find_routing_entry(routing_table, sync_msg->routing_entry.destination, sync_msg->routing_entry.mask);
                if(!node)
                {
                    printf("Deleted Entry from Routing Table: IP Addr: %s/%d, gateway:%s, OIF:%s\n", sync_msg->routing_entry.destination, 
                                sync_msg->routing_entry.mask, sync_msg->routing_entry.gway_ip, sync_msg->routing_entry.oif);
                }
                else
                {
                    printf("Failed to delete entry from routing table\n");
                }
            }
            else
            {
                printf("Entry for %s/%d is not availble to delete\n", sync_msg->routing_entry.destination, sync_msg->routing_entry.mask);
            }
            break;

        case UPDATE:
            if(node)
            {
                update_routing_entry(node, sync_msg->routing_entry.gway_ip, sync_msg->routing_entry.oif);
                node = find_routing_entry(routing_table, sync_msg->routing_entry.destination, sync_msg->routing_entry.mask);
                if(!node)
                {
                    RTEntry_t *routing_entry = (RTEntry_t *)node->data;
                    printf("Entry updated in Routing Table: IP Addr: %s/%d, gateway:%s, OIF:%s\n", routing_entry->destination, routing_entry->mask,
                                routing_entry->gway_ip, routing_entry->oif);                    
                }
                else
                {
                    printf("Failed to updated entry in routing table\n");
                }
            }
            else
            {
                printf("Entry for %s/%d is not availble to update\n", sync_msg->routing_entry.destination, sync_msg->routing_entry.mask);
            }
            break;

        default:
            printf("Invalid Operation Requested!!!\n");
            break;
    }
}


