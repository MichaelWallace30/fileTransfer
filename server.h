#pragma once
#include<stdio.h>
#include<winsock2.h>
#include<windows.h>
#include <chrono>
#include <iostream>
#include <WS2tcpip.h>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>

#define MAX_BUFFER 20000

#pragma comment(lib,"ws2_32.lib") //Winsock Library
class server
{
private:
    int socket_desc, client_sock, c, read_size;
    struct sockaddr_in serv, client;
    char bufferRecv[MAX_BUFFER];
    char bufferSend[MAX_BUFFER];

    std::mutex g_mutex;
    std::queue<std::vector<char>> recvQueue;

    bool done;
    std::thread* myThread;
    //shared vector
    std::vector<char> *otherVector;
    void serverThread();

public:
    server();
    ~server();
    bool init(std::string ip, int port);

    
    void sendVector(std::vector<char> * buffer);
    std::vector<char>* recvVector(std::vector<char> * buffer);
};

