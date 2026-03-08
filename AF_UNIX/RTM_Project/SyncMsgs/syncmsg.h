/**
 * @file syncmsg.h
 * @author Bhanu Prakash
 * @brief sync message processing helper
 * @version 0.1
 * @date 2021-12-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef _SYNC_MSG_
#define _SYNC_MSG_

#include "ll.h"
#include "rtmanager.h"

typedef struct list list_t;

typedef enum
{
    CREATE,
    UPDATE,
    DELETE,
}OPCODE;

typedef struct sync_msg
{
    OPCODE opcode;
    RTEntry_t routing_entry;
}sync_msg_t;

void process_sync_message(list_t* routing_table, sync_msg_t* sync_msg);

#endif //_SYNC_MSG_