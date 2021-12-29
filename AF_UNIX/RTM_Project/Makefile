CC = gcc
CFLAGS = -Wall -Wextra -g

DEPSRC = LL/ll.c RTManager/rtmanager.c SyncMsgs/syncmsg.c 

default: ll rtmanager syncmsg
	$(CC) $(CCFLAGS) server.c $(DEPSRC) -o server -lrt
	$(CC) $(CCFLAGS) client.c $(DEPSRC) -o client -lrt

ll:
	$(CC) $(CFLAGS) -c LL/ll.c -o LL/ll.o


rtmanager: ll
	$(CC) $(CFLAGS) -c RTManager/rtmanager.c -o  RTManager/rtmanager.o

syncmsg: ll rtmanager
	$(CC) $(CFLAGS) -c SyncMsgs/syncmsg.c -o SyncMsgs/syncmsg.o
