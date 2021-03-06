#ifndef CLINET_H
#define CLIENT_H


//#define debugClient

#ifndef __linux
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

#include<stdlib.h>
#include<stdio.h>
#include <iostream>
#include <vector>




#define MAX_BUFFER 250000

class client
{
private:
    
    int sock;
    struct sockaddr_in server;
    char bufferRecv[MAX_BUFFER];
    char bufferSend[MAX_BUFFER];


    //shared vector
    std::vector<char> *otherVector;

 
public:
    client();
    ~client();
    bool init(std::string ip, int port);


    void sendVector(std::vector<char> * buffer);
    std::vector<char>* recvVector(std::vector<char> * buffer);
};
#endif
