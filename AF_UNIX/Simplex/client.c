/**
 * @file client.c
 * @author Bhanu Prakash
 * @brief UNIX Socket implementtation for client process
 *          Establish connection with the server.
 *          Read the data(integer) and send it to server.
 *          Send 0 to server to indicate end of data items.
 *          Get the result from server and display it.
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

int main(int argc, char* argv[])
{
    int data_socket;
    struct sockaddr addr;
    char buffer[MAX_BUFSIZE];
    int i;

    //prepare params for connect call
    addr.sa_family = AF_UNIX;
    memcpy(addr.sa_data, SOCKET_NAME, sizeof(SOCKET_NAME));
    //strncpy(addr.sa_data, SOCKET_NAME, sizeof(addr.sa_data)-1);

    data_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    if(connect(data_socket, &addr, sizeof(struct sockaddr)) == -1)
    {
        perror("connect");
        printf("connect failed!!!\n");
        exit(EXIT_FAILURE);
    }

    printf("connection to server successful\n");

    do
    {
        printf("Enter data to be sent:");
        scanf("%d", &i);
        if(write(data_socket, &i, sizeof(int)) == -1)
        {
            perror("write");
            printf("write failed!!!\n");
            exit(EXIT_FAILURE);
        }
    }while(i);

    if(read(data_socket, buffer, MAX_BUFSIZE) == -1)
    {
        perror("read");
        printf("read failed!!!\n");
        exit(EXIT_FAILURE);
    }
    printf("Data Received from Server:%s\n", buffer);
    close(data_socket);
    exit(EXIT_SUCCESS);
}