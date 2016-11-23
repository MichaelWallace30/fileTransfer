#include "client.h"


client::client()
{    
    done = false;
}


client::~client()
{
    done = true;    
    if (myThread->joinable()) {
        printf("client thread trying to join\n");
        myThread->join();
        printf("client thread joined!\n");
    }

}

void client::clientThread()
{    
    while (!done)
    {
        int read_size = 0;
        while ((read_size = recv(sock, bufferRecv, MAX_BUFFER, 0)) < 0)
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


bool client::init(std::string ip, int port)
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
    

    myThread = new std::thread(&client::clientThread, this);
    return true;
}


void client::sendVector(std::vector<char> * buffer)
{
    
    memcpy(bufferSend, &(*buffer)[0], buffer->size());
    if (send(sock, bufferSend, buffer->size(), 0) < 0)
    {
        puts("Send failed");
    }
    printf("message sent\n");
}

std::vector<char>* client::recvVector(std::vector<char> * buffer)
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