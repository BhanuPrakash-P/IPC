/**
 * @file server.c
 * @author Bhanu Prakash
 * @brief Server Implementation
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
#include <signal.h>

#include "LL/ll.h"
#include "RTManager/rtmanager.h"
#include "SyncMsgs/syncmsg.h"

#define SOCKET_NAME "/tmp/tstSock"
#define MAX_BUFSIZE 128
#define  MAX_CLIENTS 10

//Server's routing table
list_t* routing_table;
int monitored_fd_list[MAX_CLIENTS];
int client_pid[MAX_CLIENTS];
int connection_socket;

void init_monitored_fd_list()
{
    int i=0;
    for(; i<MAX_CLIENTS; i++)
    {
        monitored_fd_list[i] = -1;
    }
}

void init_client_pid_list()
{
    int i=0;
    for(; i<MAX_CLIENTS; i++)
    {
        client_pid[i] = -1;
    }
}

void add_to_monitored_fd_list(int fd)
{
    int i=0;
    for(; i<MAX_CLIENTS; i++)
    {
        if(monitored_fd_list[i] != -1)
            continue;
        monitored_fd_list[i] = fd;
        break;
    }
}

void add_to_client_pid_list(int pid)
{
    int i=0;
    for(; i<MAX_CLIENTS; i++)
    {
        if(client_pid[i] != -1)
            continue;
        client_pid[i] = pid;
        break;
    }
}

void remove_from_monitored_fd_list(int fd)
{
    int i=0;
    for(; i<MAX_CLIENTS; i++)
    {
        if(monitored_fd_list[i]!= fd)
            continue;
        monitored_fd_list[i] = -1;
        break;
    }
}

void dump_monitored_fd_list()
{
    int i=0;
    for(; i<MAX_CLIENTS; i++)
    {
        printf("***FD List***\n");
        printf("%d: %d\t", i, monitored_fd_list[i]);
        printf("\n");
    }

}

void refresh_fdSet(fd_set * fdSet)
{
    FD_ZERO(fdSet);
    int i=0;
    for(; i<MAX_CLIENTS; i++)
    {
        if(monitored_fd_list[i] == -1)
            continue;
        FD_SET(monitored_fd_list[i], fdSet);
    }
}

int get_max_fd()
{
    int i = 0;
    int max = -1;
    for(; i<MAX_CLIENTS; i++)
    {
        if(monitored_fd_list[i] > max)
            max = monitored_fd_list[i];
    }
    return max;
}

void flush_routing_entries()
{
    deinit_ll(routing_table);
    routing_table = init_ll();
    for(int i=0; i<MAX_CLIENTS; i++)
    {
        if(client_pid[i] != -1)
            kill(client_pid[i], SIGUSR1);
    }
}

int create_sync_msg(char *buffer, sync_msg_t* sync_msg)
{
    char *token = strtok(buffer, " ");
    printf("operation:%s\n", token);
    switch (token[0])
    {
        case 'C': 
            sync_msg->opcode = CREATE;
            break;

        case 'U':
            sync_msg->opcode = UPDATE;
            break;

        case 'D':
            sync_msg->opcode = DELETE;
            break;
        case 'F':
            flush_routing_entries();
            return 1;
        case 'L':
            display_routing_table(routing_table);
            return 1;
        default:
            printf("Invalid operation requested in command\n");
            return -1;
            break;
    }

    token = strtok(NULL, " ");
    memcpy(sync_msg->routing_entry.destination, token, strlen(token));
    //printf("destination:%s\n", sync_msg->routing_entry.destination);


    token = strtok(NULL, " ");
    sync_msg->routing_entry.mask = atoi(token);
    //printf("mask:%d\n", sync_msg->routing_entry.mask);

    if((sync_msg->opcode == CREATE) || (sync_msg->opcode == UPDATE))
    {
        token = strtok(NULL, " ");
        memcpy(sync_msg->routing_entry.gway_ip, token, strlen(token));
        //printf("gway_ip:%s\n", sync_msg->routing_entry.gway_ip);

        token = strtok(NULL, " ");
        memcpy(sync_msg->routing_entry.oif, token, strlen(token)); 
        //printf("oif:%s\n", sync_msg->routing_entry.oif);       
    }
    return 0;
}

void update_new_client(int data_socket, list_t* routing_table)
{
    char buffer[MAX_BUFSIZE];
    sync_msg_t *sync_msg = (sync_msg_t *)malloc(sizeof(sync_msg_t));
    node_t* node = routing_table->head;
    while(node != NULL)
    {
        RTEntry_t entry = *((RTEntry_t *)node->data);
        
        sync_msg->opcode = CREATE;
        sprintf(buffer, "C %s %u %s %s", entry.destination, entry.mask, entry.gway_ip, entry.oif);

        //memcpy(sync_msg->routing_entry, entry, sizeof(entry));

        create_sync_msg(buffer, sync_msg);

        write(data_socket, sync_msg, sizeof(sync_msg_t));

        node = node->next;
    }
}

void signal_handler(int signal_num)
{
    if(signal_num == SIGINT)
    {
        for(int i=0; i<MAX_CLIENTS; i++)
        {
            if(client_pid[i] != -1)
                kill(client_pid[i], SIGINT);
        }
        deinit_ll(routing_table);
        close(connection_socket);
        remove_from_monitored_fd_list(connection_socket);
        unlink(SOCKET_NAME);
        exit(EXIT_SUCCESS);
    }
}

int main()
{
    unlink(SOCKET_NAME);

    int data_socket;
    struct sockaddr addr;
    char buffer[MAX_BUFSIZE];
    fd_set readfds;

    routing_table = init_ll();

    init_monitored_fd_list();
    init_client_pid_list();
    //dump_monitored_fd_list();
    add_to_monitored_fd_list(0);

    connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if(connection_socket == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    printf("!!!Master Socket created successfully\n");

    addr.sa_family = AF_UNIX;
    memcpy(addr.sa_data, SOCKET_NAME, strlen(SOCKET_NAME));

    if(bind(connection_socket, &addr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if(listen(connection_socket, MAX_CLIENTS) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    add_to_monitored_fd_list(connection_socket);
    //dump_monitored_fd_list();

    //Register SIGINT to close all clients
    signal(SIGINT, signal_handler);

    while(1)
    {
        refresh_fdSet(&readfds);
        memset(buffer, 0, sizeof(buffer));

        printf("\nPlease select one choice from below list\n");
        printf("1. CREATE <destination> <mask> <gateway> <oif>\n");
        printf("2. UPDATE <destination> <mask> <gateway> <oif>\n");
        printf("3. DELETE <destination> <mask>\n");
        printf("4. FLUSH(Removes all entries from Routing Table)\n");
        printf("5. LIST(Displays current routing table\n");

        select(get_max_fd()+1, &readfds, NULL, NULL, NULL);

        if(FD_ISSET(connection_socket, &readfds))
        {
            int client_pid = 0;
            data_socket = accept(connection_socket, NULL, NULL);
            if(data_socket == -1)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            add_to_monitored_fd_list(data_socket);

            //read client's pid and add it pid list
            read(data_socket, &client_pid, sizeof(int));
            add_to_client_pid_list(client_pid);

            //dump_monitored_fd_list();
            update_new_client(data_socket, routing_table);
        }
        else if(FD_ISSET(0, &readfds))
        {
            //from stdin
            //read the the routing table update by user and prepare sync_msg to be sent

            sync_msg_t * sync_msg = (sync_msg_t *)malloc(sizeof(sync_msg_t));
            memset(buffer, 0, sizeof(buffer));
            read(0, buffer, MAX_BUFSIZE);
            buffer[strcspn(buffer, "\r\n")] = 0;
            printf("Receviced command: %s\n", buffer);
            if(!create_sync_msg(buffer, sync_msg))
            {
#ifdef DEBUG
                printf("destination:%s\n", sync_msg->routing_entry.destination);
                printf("mask:%d\n", sync_msg->routing_entry.mask);
                printf("gway_ip:%s\n", sync_msg->routing_entry.gway_ip);
                printf("oif:%s\n", sync_msg->routing_entry.oif);
#endif
                process_sync_message(routing_table, sync_msg);
                display_routing_table(routing_table);

                int comm_socket = -1;
                for(int i=2; i<MAX_CLIENTS; i++)
                {
                    if(monitored_fd_list[i] == -1)
                        continue;
                    write(monitored_fd_list[i], sync_msg, sizeof(sync_msg_t));
                }
            }
        }
        else
        {
            //from existing client
            //Remove client from monitored_fd_list
            int i=2, comm_socket=-1;
            for (; i<MAX_CLIENTS; i++)
            {
                comm_socket = monitored_fd_list[i];
                if(FD_ISSET(comm_socket, &readfds))
                {
                    int done = 0;
                    if(read(comm_socket, &done, sizeof(int)) == -1)
                    {
                        perror("read");
                        exit(EXIT_FAILURE);
                    }
                    if (done == 1) 
                    { // this client is disconnecting
                        printf("Client Exited!!!\n");
                        close(comm_socket);
                        remove_from_monitored_fd_list(comm_socket);
                    }
                    else 
                    {
                        printf("%i\n", done);
                    }                    
                    
                }
            }
        }

    }

    /*close the master socket*/
    close(connection_socket);
    remove_from_monitored_fd_list(connection_socket);
    printf("connection closed..\n");

    unlink(SOCKET_NAME);
    exit(EXIT_SUCCESS);
}