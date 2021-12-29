/**
 * @file server.c
 * @author Bhanu Prakash
 * @brief UNIX Socket implementation for server process with multiplexing support
 *          Server process will wait for client connections.
 *          Receives and adds the data from client, until 0 is sent by client.
 *          send the result(sum of all the data items received) to client.
 * @version 0.1
 * @date 2021-12-26
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

#define SOCKET_NAME "/tmp/tstSock"
#define MAX_BUFSIZE 128
#define MAX_CLIENTS 10

int monitored_fd_list[MAX_CLIENTS];

int client_result[MAX_CLIENTS] = {0};

void init_monitored_fd_list()
{
    int i=0;
    for(; i<MAX_CLIENTS; i++)
    {
        monitored_fd_list[i] = -1;
    }
}

void add_to_monitored_fd_list(int sockfd)
{
    int i=0;
    for(; i<MAX_CLIENTS; i++)
    {
        if(monitored_fd_list[i] != -1)
            continue;
        monitored_fd_list[i] = sockfd;
        break;
    }
}

void remove_from_monitored_fd_list(int sockfd)
{
    int i=0;
    for(; i<MAX_CLIENTS; i++)
    {
        if(monitored_fd_list[i] != sockfd)
            continue;
        monitored_fd_list[i] = -1;
        break;
    }
}

void refresh_fdSet(fd_set* fdSet_sock)
{
    //Remove existing FDs from fd_set data structure
    FD_ZERO(fdSet_sock);
    int i=0;
    for(; i<MAX_CLIENTS; i++)
    {
        if(monitored_fd_list[i] == -1)
            continue;
        
        FD_SET(monitored_fd_list[i], fdSet_sock);
    }
}

int get_max_fd()
{
    int i=0, max=-1;
    for(; i<MAX_CLIENTS; i++)
    {
        if(monitored_fd_list[i]>max)
            max = monitored_fd_list[i];
    }
    return max;
}


int main(int argc, char* argv[])
{
    //check and close if socket exists already
    unlink(SOCKET_NAME);

    int connection_socket;
    int data_socket;
    struct sockaddr addr;
    fd_set readfds;
    char buffer[MAX_BUFSIZE];
    int result = 0;
    int data = 0;
    char res[MAX_BUFSIZE];

    //initializing monitoring fd list
    init_monitored_fd_list();
    add_to_monitored_fd_list(0);

    //creating master socket
    connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if(connection_socket == -1)
    {
        perror("master_socket");
        exit(EXIT_FAILURE);
    }
    printf("!!!Master socket created!!!\n");

    //prepare params for bind call
    addr.sa_family = AF_UNIX;
    memcpy(addr.sa_data, SOCKET_NAME, sizeof(SOCKET_NAME));
    //strncpy(addr.sa_data, SOCKET_NAME, sizeof(addr.sa_data)-1);

    if(bind(connection_socket, &addr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    printf("Bind successful!!!\n");

    if(listen(connection_socket, MAX_CLIENTS) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    add_to_monitored_fd_list(connection_socket);

    while(1)
    {
        refresh_fdSet(&readfds);

        select(get_max_fd()+1, &readfds, NULL, NULL, NULL);

        if(FD_ISSET(connection_socket, &readfds))
        {
            //server received connection initiation request from new client
            printf("Waiting for client connection request!!!\n");
            data_socket = accept(connection_socket, NULL, NULL);

            if(data_socket == -1)
            {
                perror("data_socket");
                exit(EXIT_FAILURE);
            }
            printf("Accepted client connection request!!!\n");
            add_to_monitored_fd_list(data_socket);
        }
        else if(FD_ISSET(0, &readfds))
        {
            memset(buffer, 0, sizeof(buffer));
            read(0, buffer, MAX_BUFSIZE);
            buffer[strcspn(buffer, "\r\n")] = 0; // flush new line char from 
            printf("Received data=%s from stdin\n", buffer);
        }
        else
        {
            int i=0, comm_socket=-1, data=0;
            for(; i<MAX_CLIENTS; i++)
            {
                if(FD_ISSET(monitored_fd_list[i], &readfds))
                {
                    comm_socket = monitored_fd_list[i];
                    
                    memset(buffer, 0, MAX_BUFSIZE);
                    printf("Waiting for data from client!!!\n");
                    if(read(comm_socket, buffer, MAX_BUFSIZE) == -1)
                    {
                        perror("read");
                        exit(EXIT_FAILURE);
                    }
                    
                    memcpy(&data, buffer, sizeof(int));

                    printf("Received data=%d for client%d\n", data, i);
                    if(data == 0)
                    {
                        memset(res, 0, sizeof(res));
                        snprintf(res, sizeof(res), "Result = %d", client_result[i]);
                        printf("sending result to client%d:%s\n", i, res);

                        if(write(comm_socket, res, sizeof(res)) == -1)
                        {
                            perror("write");
                            exit(EXIT_FAILURE);
                        }        
                        close(comm_socket);
                        client_result[i] = 0;
                        remove_from_monitored_fd_list(comm_socket); 
                        continue;                  
                    }
                    client_result[i] += data; 
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