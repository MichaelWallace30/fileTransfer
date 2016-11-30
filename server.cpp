#include "server.h"


server::server()
{    
       
}


server::~server()
{

}


bool server::init(std::string ip, int port)
{
#ifndef __linux
    WSADATA wsa;
    //Initialise winsock
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return false;
    }
    printf("Initialised.\n");
#endif

    //Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    serv.sin_family = AF_INET;    
    const char* tempclient_ip = ip.c_str();
    serv.sin_addr.s_addr = inet_addr(tempclient_ip);
    serv.sin_port = htons(port);

    //Bind
    if (bind(socket_desc, (struct sockaddr *)&serv, sizeof(serv)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return false;
    }
    puts("bind done");

    //Listen
    listen(socket_desc, 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
        perror("accept failed");
        return false;
    }
    puts("Connection accepted");

    
    return true;
}


void server::sendVector(std::vector<char> * buffer)
{
    memcpy(bufferSend, &(*buffer)[0], buffer->size());
    if (send(client_sock, bufferSend, buffer->size(), 0) < 0)
    {
        printf("Send failed\n");
    }
#ifdef debugServer
    printf("server send\n");
#endif
}

std::vector<char>* server::recvVector(std::vector<char> * buffer)
{    
	int read_size = 0;
	while ((read_size = recv(client_sock, bufferRecv, MAX_BUFFER, 0)) < 0)
	{
	}


    //deep copy
    buffer->clear();
    for (int x = 0; x < read_size; x++)
    {
        buffer->push_back(bufferRecv[x]);
    }
    
    return buffer;
}

