
cmake_minimum_required(VERSION 3.0)
project("ftp")

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread")
get_filename_component(PARENT_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)




add_executable(ftp
base64.h
client.h
server.h
hash.h
encryption.h


packetHeader.h
fileManager.h
userManager.h

base64.cpp
client.cpp
server.cpp

hash.cpp
encryption.cpp

fileManager.cpp
userManager.cpp

#main_encryption_encode.cpp
#main_client_server.cpp
#main_hash.cpp
main.cpp

user.txt
key.txt
)


