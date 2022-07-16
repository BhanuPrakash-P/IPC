## Implementing Several IPC mechanisms.

1. Client-Server communication using UNIX sockets.
    - Client Process: Reads the data from user and sends it to server until user provides input '0'.
    - Server Process: Gets the data from Client process and adds the all data items received and sends the result back to client upon receiving '0'.

2. Client-Server multiplex communication using UNIX sockets.

3. Routing Table Management Project
    - TODO: Fix loop issues and add signal handlers for proper termination of client and server processes

4. Basic Message Queue Implementaion
    - TODO: Add signal handlers for destorying/removing message queue

**BUILD Instructions:**
CMake support is available to build server and client applications.
After cloning the Repo, run the following command to generate the Makefile
```cmake . -B build```
Makefile gets generated under build directoty. So, goto build directory and run ```make``` command.
This will compile the code and generate required libraries and server, client applications.
