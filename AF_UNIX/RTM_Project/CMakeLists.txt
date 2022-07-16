cmake_minimum_required (VERSION 2.6)
project (RTMManager)

include_directories ("LL")
include_directories ("RTManager")
include_directories ("SyncMsgs")

add_subdirectory( LL )
add_subdirectory( SyncMsgs )
add_subdirectory( RTManager )

add_executable(server server.c)
add_executable(client client.c)

TARGET_LINK_LIBRARIES(server ll rtmanager syncmsg)
TARGET_LINK_LIBRARIES(client ll rtmanager syncmsg)