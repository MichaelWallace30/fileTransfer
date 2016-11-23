#include "server.h"


server::server()
{    
    done = false;        
}


server::~server()
{
    done = true;    
    if (myThread->joinable()) {
        printf("client thread trying to join\n");
        myThread->join();
        printf("client thread joined!\n");
    }
}


bool server::init(std::string ip, int port)
{
#ifdef WIN32
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

    myThread = new std::thread(&server::serverThread, this);
    return true;
}

void server::serverThread()
{

    while (!done)
    {
        int read_size = 0;
        while ((read_size = recv(client_sock, bufferRecv, MAX_BUFFER, 0)) < 0)
        {
        }

        if (read_size != 0)
        {
            
            std::vector<char> tempVector;            
            //deep copy
            for (int x = 0; x < read_size; x++)
            {
                tempVector.push_back(bufferRecv[x]);
            }

            g_mutex.lock();
            recvQueue.push(tempVector);
            g_mutex.unlock();

            
            printf("server recv\n");
        }
        
    }
}

void server::sendVector(std::vector<char> * buffer)
{
    memcpy(bufferSend, &(*buffer)[0], buffer->size());
    if (send(client_sock, bufferSend, buffer->size(), 0) < 0)
    {
        printf("Send failed\n");
    }
    printf("server send\n");
}

std::vector<char>* server::recvVector(std::vector<char> * buffer)
{    
    while (recvQueue.size() == 0); //just wait for ever
    
    std::vector<char> temp = recvQueue.front();
    g_mutex.lock();
    recvQueue.pop();//mutex
    g_mutex.unlock();    
    //deep copy
    buffer->resize(0);
    for (int x = 0; x < temp.size(); x++)
    {
        buffer->push_back(temp[x]);
    }
    
    return buffer;
}

