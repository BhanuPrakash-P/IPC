Implementing Several IPC mechanisms.

1. Client-Server communication using UNIX sockets.
    Client Process: Reads the data from user and sends it to server until user provides input '0'.
    Server Process: Gets the data from Client process and adds the all data items received and sends the result back to client upon receiving '0'.