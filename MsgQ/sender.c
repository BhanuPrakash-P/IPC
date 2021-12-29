/**
 * @file sender.c
 * @author Bhanu Prakash
 * @brief Sender Process Implementation
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

#define MAX_BUFSIZE 256

int main(int argc, char** argv)
{
    char buffer[MAX_BUFSIZE];
    if(argc < 2)
    {
        printf("Please enter msg name. Example </msg-name>\n");
        return 0;
    }

    int MQid;

    if((MQid = mq_open(argv[1], O_CREAT | O_WRONLY, 0 ,0)) == -1)
    {
        perror("sender: mq_open");
        exit(EXIT_FAILURE);
    }

    printf("Please enter a message!!!\n");
    scanf("%s", buffer);

    if(mq_send(MQid, buffer, strlen(buffer)+1, 0 == -1))
    {
        perror("sender: mq_send");
        exit(EXIT_FAILURE);
    }

    printf("Message sent to receiver!!!\t I'm Done!!! Exiting!!!\n");
    if(mq_close(MQid))
    {
        perror("sender: mq_close");
        exit(EXIT_FAILURE);
    }
}
