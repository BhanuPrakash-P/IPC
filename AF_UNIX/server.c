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
    //check and close if socket exists already
    unlink(SOCKET_NAME);

    int connection_socket;
    int data_socket;
    struct sockaddr addr;
    char buffer[MAX_BUFSIZE];
    int result = 0;
    int data = 0;
    char res[MAX_BUFSIZE];

    #if 0
        //sockaddr structure for reference
        struct sockaddr{
            sa_family_t sa_family;
            char sa_data[14];
        };
    #endif

    //creating master socket
    connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if(connection_socket == -1)
    {
        perror("master_socket");
        printf("Master socket creation failed!!!\n");
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
        printf("Bind failed!!!\n");
        exit(EXIT_FAILURE);
    }
    printf("Bind successful!!!\n");

    if(listen(connection_socket, 5) == -1)
    {
        perror("listen");
        printf("listen failed!!!\n");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        //creating data socket
        printf("Waiting for client connection request!!!\n");
        data_socket = accept(connection_socket, NULL, NULL);

        if(data_socket == -1)
        {
            perror("data_socket");
            printf("data_socket creation failed!!!\n");
            exit(EXIT_FAILURE);
        }

        printf("Accepted client connection request!!!\n");

        while(1)
        {
            memset(buffer, 0, sizeof(buffer));
            data = 0;
            printf("Waiting for data from client!!!\n");
            if(read(data_socket, buffer, MAX_BUFSIZE) == -1)
            {
                perror("read");
                printf("read failed!!!\n");
                exit(EXIT_FAILURE);
            }
            
            memcpy(&data, buffer, sizeof(int));
            printf("Received data=%d\n", data);
            if(data == 0)
                break;
            result = result + data;
        }
        memset(res, 0, sizeof(res));
        snprintf(res, sizeof(res), "Result = %d", result);
        printf("sending result to Client:%s\n", res);
        
        if(write(data_socket, res, sizeof(res)) == -1)
        {
            perror("write");
            printf("write failed!!!\n");
            exit(EXIT_FAILURE);
        }        
        close(data_socket);
    }
    /*close the master socket*/
    close(connection_socket);
    printf("connection closed..\n");

    unlink(SOCKET_NAME);
    exit(EXIT_SUCCESS);

}