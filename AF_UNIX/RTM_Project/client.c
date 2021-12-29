/**
 * @file client.c
 * @author Bhanu Prakash
 * @brief Client Implementation
 * @version 0.1
 * @date 2021-12-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include "LL/ll.h"
#include "RTManager/rtmanager.h"
#include "SyncMsgs/syncmsg.h"

#define SOCKET_NAME "/tmp/tstSock"
#define MAX_BUFSIZE 128

int main()
{
    int data_socket;
    struct sockaddr addr;
    int buffer[MAX_BUFSIZE];
    //sync_msg_t* sync_msg;

    list_t* routing_table = init_ll();

    //prepare params for connect call
    addr.sa_family = AF_UNIX;
    memcpy(addr.sa_data, SOCKET_NAME, strlen(SOCKET_NAME));

    data_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if(data_socket == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if(connect(data_socket, &addr, sizeof(struct sockaddr)) == -1)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        sync_msg_t* sync_msg = (sync_msg_t *)malloc(sizeof(sync_msg_t));
        printf("Client is waiting for data!!!\n");
        if(read(data_socket, sync_msg, sizeof(sync_msg_t)) == -1)
        {
            perror("read");
            exit(EXIT_FAILURE);
        }
        
#ifdef DEBUG
        printf("destination:%s\n", sync_msg->routing_entry.destination);
        printf("mask:%d\n", sync_msg->routing_entry.mask);
        printf("gway_ip:%s\n", sync_msg->routing_entry.gway_ip);
        printf("oif:%s\n", sync_msg->routing_entry.oif);
#endif

        process_sync_message(routing_table, sync_msg);
        display_routing_table(routing_table);
    }
}