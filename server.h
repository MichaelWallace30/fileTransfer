#ifndef SERVER_H
#define SERVER_H
#ifdef WIN32
	#include<winsock2.h>
	#include<windows.h>
	#include <WS2tcpip.h>
	#pragma comment(lib,"ws2_32.lib") //Winsock Library
#else
	#include <errno.h>
	#include <string.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <sys/wait.h>
	#include <signal.h>
#endif


#include<stdio.h>
#include <chrono>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>

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
#endif