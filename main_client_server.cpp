#include "client.h"
#include "server.h"
#include <thread>



int main()
{

    std::vector<char>* myVector = new std::vector<char>();

    for (int x = 0; x < 10; x++)
    {
        myVector->push_back(x);
    }
    /*
    
    server  myServer;
    myServer.init();
    */
    
   client myServer;
   myServer.init("192.168.1.121", 8888);
    
  
    
   
    while (true)
    {
        myServer.sendVector(myVector);
        do
        {
            myVector = myServer.recvVector(myVector);
        } while (myVector->size() == 0);

      

    }
    
    
    
}