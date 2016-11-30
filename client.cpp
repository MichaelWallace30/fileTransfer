#include "client.h"


client::client()
{    

}


client::~client()
{


}


bool client::init(std::string ip, int port)
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
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
    
    server.sin_family = AF_INET;
    const char* tempclient_ip = ip.c_str();
    server.sin_addr.s_addr = inet_addr(tempclient_ip);//the servers ip
    
    server.sin_port = htons(port);

    //Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return false;
    }

    puts("Connected\n");
    

    
    return true;
}


void client::sendVector(std::vector<char> * buffer)
{
    
    memcpy(bufferSend, &(*buffer)[0], buffer->size());
    if (send(sock, bufferSend, buffer->size(), 0) < 0)
    {
        puts("Send failed");
    }
#ifdef debugClient
    printf("message sent\n");
#endif
}

std::vector<char>* client::recvVector(std::vector<char> * buffer)
{
	int read_size = 0;
	while ((read_size = recv(sock, bufferRecv, MAX_BUFFER, 0)) < 0)
	{
	}
    buffer->clear();
    for (int x = 0; x < read_size; x++)
    {
        buffer->push_back(bufferRecv[x]);
    }

    return buffer;
}