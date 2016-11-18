#pragma once
#include<stdio.h>
#include<winsock2.h>
#include<windows.h>
#include <chrono>
#include <iostream>
#include <WS2tcpip.h>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define MAX_BUFFER 20000

class client
{
private:
    
    int sock;
    struct sockaddr_in server;
    char bufferRecv[MAX_BUFFER];
    char bufferSend[MAX_BUFFER];

    std::mutex g_mutex;
    std::queue<std::vector<char>> recvQueue;

    bool done;
    std::thread* myThread;
    //shared vector
    std::vector<char> *otherVector;
    void clientThread();
 
public:
    client();
    ~client();
    bool init(std::string ip, int port);


    void sendVector(std::vector<char> * buffer);
    std::vector<char>* recvVector(std::vector<char> * buffer);
};

