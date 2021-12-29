/**
 * @file receiver.c
 * @author Bhanu Prakash
 * @brief Receiver Process Implementation
 * @version 0.1
 * @date 2021-12-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <signal.h>

#define MAX_MSG_NUM 5
#define MAX_BUFSIZE 256

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        printf("Please enter msg name. Example </msg-name>\n");
        return 0;  
    }

    int MQid;
    fd_set readfds;
    struct mq_attr attr;
    char buffer[MAX_BUFSIZE];

    attr.mq_maxmsg = MAX_MSG_NUM;
    attr.mq_msgsize = MAX_BUFSIZE;
    attr.mq_curmsgs = 0;
    attr.mq_flags = 0;

    if((MQid = mq_open(argv[1], O_CREAT | O_RDONLY, 0660 , &attr)) == -1)
    {
        perror("sender: mq_open");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        FD_ZERO(&readfds);
        FD_SET(MQid, &readfds);

        printf("Waiting for message!!\n");
        select(MQid+1, &readfds, NULL, NULL, NULL);

        if(FD_ISSET(MQid, &readfds))
        {
            memset(buffer, 0, MAX_BUFSIZE);

            if(mq_receive(MQid, buffer, MAX_BUFSIZE, NULL) == -1)
            {
                perror("receiver: mq_receive");
                exit(EXIT_FAILURE);
            }
            printf("Received Message:%s\n", buffer);         
        }
    }
}
